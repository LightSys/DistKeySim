#!/usr/bin/env python3

import json
import re
import sys

if __name__ == '__main__':

    namePat = re.compile('(\w+): ')
    valuePat = re.compile(': (\w+),')

    for line in sys.stdin:
        if 'sendMsg: {' in line or \
           'receiveMessage: {' in line or \
           'getHeartbeatMessage: {' in line or \
           'canSendKeyspace: {' in line or \
           'adak: {' in line:
            parts = line.split(':')
            word = parts[0]
        else:
            print(line.rstrip())
            continue

        line1 = namePat.sub('"\\1": ', line.rstrip().replace(word + ': ', ''))
        line2 = valuePat.sub(': "\\1",', line1)
        line3 = line2.replace('timestamp', '"timestamp":')\
                     .replace('info', '"info":')\
                     .replace('records', '"records":')\
                     .replace('creationRateData', '"creationRateData":')\
                     .replace(', }', '}')\
                     .replace('keyspaces', '"keyspaces":')\
                     .replace('keyspace ', '"keyspace": ')
        j = json.loads(line3)
        print(word, ":", json.dumps(j, indent=4))
