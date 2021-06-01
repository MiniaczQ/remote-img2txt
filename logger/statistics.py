import sys, os

class Frame:
    def __init__(self, idx, clk, precam, poscam, pretxt, postxt, cons):
        self.index = idx
        self.clock = clk
        self.preCamera = precam
        self.postCamera = poscam
        self.preASCII = pretxt
        self.postASCII = postxt
        self.console = cons
    
    def __str__(self):
        return f'{self.index:5};{self.clock:.3f};{self.preCamera:.3f};{self.postCamera:.3f};{self.preASCII:.3f};{self.postASCII:.3f};{self.console:.3f}'

class DiffFrame:
    def __init__(self, frame):
        self.clkToCam = frame.preCamera - frame.clock
        self.cam = frame.postCamera - frame.preCamera
        self.camToTxt = frame.preASCII - frame.postCamera
        self.txt = frame.postASCII - frame.preASCII
        self.txtToCon = frame.console - frame.postASCII
    
    def __str__(self):
        return f'{self.clkToCam:.3f};{self.cam:.3f};{self.camToTxt:.3f};{self.txt:.3f};{self.txtToCon:.3f}'

def parseFrames(log):
    frames = []
    with open(log, 'r') as f:
        for l in f:
            if l != '':
                values = l.strip().split(';')[:-1]
                frame = Frame(*[float(v) / 1000000 for v in values])
                frames.append(frame)
    return frames

def sumUpLog(log):
    frames = parseFrames(log)
    for f in frames:
        df = DiffFrame(f)
        print(df)

def main():
    logs = list(filter(lambda n: n.endswith('.txt'), os.listdir()))
    for log in logs:
        sumUpLog(log)

if __name__ == '__main__':
    main()
