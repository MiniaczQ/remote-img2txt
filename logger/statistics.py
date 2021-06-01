import sys, os

class Frame:
    def __init__(self, idx, clk, precam, poscam, pretxt, postxt, cons):
        index = idx
        clock = clk
        preCamera = precam
        postCamera = poscam
        preASCII = pretxt
        postASCII = postxt
        console = cons

class DiffFrame:
    def __init__(frame):
        

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
        print(f)

def main():
    logs = list(filter(lambda n: n.endswith('.txt'), os.listdir()))
    for log in logs:
        sumUpLog(log)

if __name__ == '__main__':
    main()
