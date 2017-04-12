import random
import textwrap


def gen_T(n): 
    l = list(range(n))
    random.shuffle(l)
    return '\n    '.join(textwrap.wrap('unsigned T{0}[{0}] = '.format(n) 
        + str(l).replace('[','{').replace(']','}'))) + ';\n\n'

def gen_hash(n):
    return """unsigned hash{0}(const char* s) {{
    unsigned h = 0;
    for (int c = *s++; c; c = *s++) {{
        h = T{0}[h ^ ({1} & c)];
    }}
    return h;
}}\n\n""".format(n, n - 1)


random.seed(0)
with open('pearson.h','a') as p:
    r = range(7, 13 + 1)
    for n in r: 
        print(2 ** n)
        p.write(gen_T(2 ** n))

    for n in r: 
        p.write(gen_hash(2 ** n))
