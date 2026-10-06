RFCTEXT = '''
        00 A            17 R            34 i            51 z
        01 B            18 S            35 j            52 0
        02 C            19 T            36 k            53 1
        03 D            20 U            37 l            54 2
        04 E            21 V            38 m            55 3
        05 F            22 W            39 n            56 4
        06 G            23 X            40 o            57 5
        07 H            24 Y            41 p            58 6
        08 I            25 Z            42 q            59 7
        09 J            26 a            43 r            60 8
        10 K            27 b            44 s            61 9
        11 L            28 c            45 t            62 +
        12 M            29 d            46 u            63 /
        13 N            30 e            47 v
        14 O            31 f            48 w         (pad) =
        15 P            32 g            49 x
        16 Q            33 h            50 y
'''

a = ''
for i in range(0, len(RFCTEXT)):
    if RFCTEXT[i] == RFCTEXT[i].strip():
        a = a + RFCTEXT[i]

convertToMap = a.replace('(pad)=','')

segs = []
for i in range(0, len(convertToMap)-1):
    if i % 3 == 0:
        segs.append(convertToMap[i:i+3])

m = {}
for i in range(0, len(segs)):
    m[int(segs[i][:2])] = segs[i][2:]

arr = []

for i in range(0, 64):
    arr.append(m[i])

print(f'char arr[64] = {arr};')
