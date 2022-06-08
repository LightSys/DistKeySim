#!/usr/bin/env python3

import json
import re
import sys

if __name__ == '__main__':

    namePat = re.compile('(\w+): ')
    valuePat = re.compile(': (\w+),')

    for line in sys.stdin:
        if 'sendMsg' not in line:
            print(line.rstrip())
            continue

        line1 = namePat.sub('"\\1": ', line.rstrip().replace('sendMsg: ', ''))
        line2 = valuePat.sub(': "\\1",', line1)
        line3 = line2.replace('timestamp', '"timestamp":')\
                     .replace('info', '"info":')\
                     .replace('records', '"records":')\
                     .replace('creationRateData', '"creationRateData":')\
                     .replace(', }', '}')\
                     .replace('keyspaces', '"keyspaces":')\
                     .replace('keyspace ', '"keyspace": ')
        j = json.loads(line3)
        print(json.dumps(j, indent=4))
