
import sys, random, math

def gen_for_thread(n, scenario):
    cmds = []

    if scenario == "variant":
        probs = [('read0', 20), ('write0',5), ('read1',20), ('write1',5), ('string',50)]
    elif scenario == "uniform":
       
        probs = [('read0',20), ('write0',20), ('read1',20), ('write1',20), ('string',20)]
    elif scenario == "skewed":
  
        probs = [('read0',5), ('write0',60), ('read1',5), ('write1',5), ('string',25)]
    else:
        raise ValueError("unknown scenario")

    total = sum(p for _,p in probs)
    weights = [p/total for _,p in probs]
    types = [t for t,_ in probs]

    for _ in range(n):
        typ = random.choices(types, weights)[0]
        if typ == 'read0':
            cmds.append("read 0")
        elif typ == 'write0':
            cmds.append("write 0 1")
        elif typ == 'read1':
            cmds.append("read 1")
        elif typ == 'write1':
            cmds.append("write 1 1")
        elif typ == 'string':
            cmds.append("string")
    return cmds

def write_file(name, cmds):
    with open(name, 'w') as f:
        f.write("\n".join(cmds))

if __name__ == "__main__":
    if len(sys.argv) < 4:
        print("Usage: gen_commands.py scenario num_cmds out_prefix")
        sys.exit(1)
    scenario = sys.argv[1]
    n = int(sys.argv[2])
    prefix = sys.argv[3]
    
    for t in range(3):
        cmds = gen_for_thread(n, scenario)
        write_file(f"{prefix}_thread{t}.txt", cmds)
    print("Generated files:", [f"{prefix}_thread{t}.txt" for t in range(3)])
