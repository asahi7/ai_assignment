#!/usr/bin/env python3.4

# Author: Tsz-Chiu Au

import sys
import subprocess
import functools

def print_usage():
    print("Usage:")
    print()
    print("      ", sys.argv[0], "spec.txt [program1|-] [program2|-]")
    print()

if len(sys.argv) != 4:
    print_usage()
    exit(1)

specfile = sys.argv[1]
p1file = sys.argv[2]
p2file = sys.argv[3]

spec = [ int(i) for line in open(specfile) for i in line.split()]

sizeX = spec[0]
sizeY = spec[1]

board = [ [ spec[2 + x + y * sizeX] for x in range(sizeX) ] for y in range(sizeY) ]

rules = []
i = 2+sizeX*sizeY;
while spec[i] != -1:
    rules.append(spec[(i+1):(i+1+spec[i])])
    i = i + spec[i] + 1

board_space_num = sum(list(map(lambda x: 1 if x>=0 else 0, spec[2:(2+sizeX*sizeY)])))

def print_divider(board):
    for x in range(sizeX):
        print('-', end='')

def print_state(state, board):
    for y in range(sizeY):
        for x in range(sizeX):
            if board[y][x] != -1:
                if state[x + y * sizeX] > 0:
                    print('O', end='')
                elif state[x + y * sizeX] < 0:
                    print('X', end='')
                else:
                    print('*', end='')
            else:
                print('#', end='')
        print()

def check_rules(state, move, rules):
    l = [ functools.reduce((lambda x, y: x and y), [ (state[i] == move) for i in r ], True) for r in rules ]
    return functools.reduce((lambda x, y: x or y), l, False)

def count_move(state):
    return sum(list(map(lambda x: 1 if x!=0 else 0, state)))

def get_user_move(state, board, prefix):
    while True:
        a = [ int(i) for i in input(prefix).split() ]
        move = a[0] + a[1] * sizeX;
        if 0 <= a[0] and a[0] < sizeX and 0 <= a[1] and a[1] < sizeY and state[move] == 0 and board[int(move/sizeX)][move % sizeX] >= 0:
            return move
        else:
            print("Invalid input.")


if p1file != '-':
    player1 = subprocess.Popen([p1file, '-p1', specfile], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
if p2file != '-':
    player2 = subprocess.Popen([p2file, '-p2', specfile], stdin=subprocess.PIPE, stdout=subprocess.PIPE)

state = [ 0 for i in range(sizeX * sizeY) ]


print_divider(board)
print(" START")
print_state(state, board)

while True:
    if p1file != '-':
        try:
            move1 = int(player1.stdout.readline().decode('UTF-8'))
        except (IOError, ValueError):
          break
    else:
        move1 = get_user_move(state, board, "Enter Player 1's move: ")
    if move1 < 0 or move1 >= sizeX * sizeY:
        print("Incorrect input: " + str(move1))
        break
    state[move1] = 1
    print_divider(board)
    print(" Player 1 chose ({0},{1})".format((move1 % sizeX), int(move1 / sizeX)))
    print_state(state, board)
    if check_rules(state, 1, rules):
        print_divider(board)
        print(" Player 1 won.")
        break
    if p2file != '-':
        try:
            player2.stdin.write(bytes("%d\n" % move1, 'UTF-8'))
            player2.stdin.flush()
        except IOError:
            break
    if count_move(state) >= board_space_num:
        print_divider(board)
        print(" DRAW")
        break;

    if p2file != '-':
        try:
            move2 = int(player2.stdout.readline().decode('UTF-8'))
        except (IOError, ValueError):
            break
    else:
        move2 = get_user_move(state, board, "Enter Player 2's move: ")
    if move2 < 0 or move2 >= sizeX * sizeY:
        print("Incorrect input: " + str(move2))
        break
    state[move2] = -1
    print_divider(board)
    print(" Player 2 chose ({0},{1})".format((move2 % sizeX), int(move2 / sizeX)))
    print_state(state, board)
    if check_rules(state, -1, rules):
        print_divider(board)
        print(" Player 2 won.")
        break
    if p1file != '-':
        try:
            player1.stdin.write(bytes("%d\n" % move2, 'UTF-8'))
            player1.stdin.flush()
        except IOError:
            break
    if count_move(state) >= board_space_num:
        print_divider(board)
        print(" DRAW")
        break;


if p1file != '-':
    player1.kill()

if p2file != '-':
    player2.kill()


