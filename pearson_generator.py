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
    for (int i = 0; s[i]; i += {2}) {{
        h = T{0}[h ^ ({1} & (int)({3}))];
    }}
    return h;
}}\n\n""".format(2 ** n, 2 ** n - 1, 2 ** (n - 8),
        " + ".join("s[i{}]".format(
            '' if i == 0 else ' + ' + str(i)) 
        for i in range(pow(2, (n - 8))))
    )


random.seed(0)
with open('pearson.h','a') as p:
    # Cannot be smaller than 8
    r = range(8, 13 + 1)
    for n in r: 
        p.write(gen_T(2 ** n))

    for n in r: 
        p.write(gen_hash(n))
