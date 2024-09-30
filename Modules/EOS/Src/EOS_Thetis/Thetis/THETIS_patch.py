#! /usr/bin/python

import os, re

listProtos = {}

def removeC_Comments(s):
    t = s;
    while len(t) > 0:
        p0 = t.find('/*')
        if p0 >= 0:
            p1 = t.find('*/', p0)
            if p1 >= 0:
                t = t[:p0] + t[p1+2:]
        else:
            return t

for root, dirs, files in os.walk(".", topdown=False):
    for name in files:
        if name.endswith('.h'):
            print('>>>', name)
            with open(os.path.join(root, name)) as fIn:
                s = fIn.read()
            s = removeC_Comments(s)
            while len(s) > 0:
                m = re.search(r"(int|void|REAL)[ \t\n]*", s)
                if m:
                    r = m.group(1)
                    s = s[m.span()[1]:]
                    m = re.match(r"([a-zA-Z0-9_]+)[ \t\n]*", s)
                    if m:
                        name = m.group(1)
                        s = s[m.span()[1]:]
                        m = re.match(r"\(([a-zA-Z0-9_, \n\t\*]+)\)[ \t\n]*;", s)
                        if m:
                            types = m.group(1).split(',')
                            s = s[m.span()[1]:]
                            for k in range(len(types)):
                                types[k] = types[k].strip()
                            listProtos[name] = (r, name, types)
                            print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>             ", listProtos[name]);
                else:
                    break
print('ok 1')
for root, dirs, files in os.walk(".", topdown=False):
    for name in files:
        if name.endswith('.c'):
            if name == 'TH_fct.c':
                continue
            print('>>>>', name)
            with open(os.path.join(root, name)) as fIn:
                s = fIn.read()
            p = 0
            while p < len(s):
                m = re.search(r"(int|void|REAL)[ \t\n]*", s)
                if m:
                    r = m.group(1)
                    s = s[m.span()[1]:]
                    m = re.match(r"([a-zA-Z0-9_]+)[ \t\n]*", s)
                    if m:
                        name = m.group(1)
                        s = s[m.span()[1]:]
                        m = re.match(r"\(([a-zA-Z0-9_, \n\t\*]+)\)[ \t\n]*;", s)
                        if m:
                            types = m.group(1).split(',')
                            s = s[m.span()[1]:]
                            for k in range(len(types)):
                                types[k] = types[k].strip()
                            listProtos[name] = (r, name, types)
                            print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>             ", listProtos[name]);
                else:
                    break
            p = 0
            while p < len(s):
                m = re.search(r"(int|void|REAL)[ \t\n]+([a-zA-Z0-9_]+)[ \t\n]*\(([^\(]*)\)", s[p:])
                print(p, m)
                if m:
                    r = m.group(1)
                    name = m.group(2)
                    params = m.group(3).replace(' ', '').split(',')
                    for k in range(len(params)):
                        params[k] = params[k].strip()
                    p += m.span()[1]
                    print('<', r, '>', '<', name, '>', '<', params, '>')
                    if name in listProtos:
                        pr = listProtos[name]
                        types = pr[2]
                        t = r + ' ' + name + '('
                        n = len(types)
                        for k in range(n):
                            t += types[k] + ' ' + params[k]
                            if k < n-1:
                                t += ', '
                        t += ')'
                        p = m.span()[1]
                        print('>>>>>>>>', t)
                else:
                    break

    