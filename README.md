# Chess Engine

A Light Weight UCI-compatible chess engine written in C++, capable of playing at approximately ~1800 rating.

## Demo
https://github.com/user-attachments/assets/00e59224-90a1-41a8-8675-429a33e16305

## Features

### Search
- **Alpha-Beta Pruning** with fail-soft framework
- **Quiescence Search** to resolve tactical sequences
- **Transposition Table** with Zobrist hashing for position caching
- **Check Extensions** to avoid horizon effect in tactical positions
- **Iterative Deepening** for time management

### Move Ordering
- **TT Move Ordering** - try the best move from transposition table first
- **MVV-LVA** (Most Valuable Victim - Least Valuable Attacker) for capture ordering

### Evaluation
- **Material:** Evaluation with standard piece values
- **Piece-Square:** Tables with phase interpolation (opening → endgame)
- **Mobility Evaluation:** for bishops and rooks
- **Passed Pawn:** Detection with rank-based bonuses
- **King Safety:** pawn shield evaluation for castled kings
- **Development Penalties:** bishop blocking central pawns

### Performance
| Depth | Time | Nodes |
|-------|------|-------|
| 5 | ~660ms | ~68k |
| 6 | ~6.7s | ~263k |
| 7 | ~21s | ~1.6M |


### Favourite Opening
#### White
Queen's Pawn Opening: Accelerated London System (1. d4 d5 2. Bf4 Nf6 3. e3 e6 4. Bd3)

#### Black
Against King's Pawn Opening -> Two Knight's Defense (1. e4 e5 2. Nc3, Nf6 3. Bc4 Nc6)

Against Queen's Gambit -> Queen's Gambit Accepted (1. d4 d5 2.c4 dxc4 3. Nf3 b5)

## Building

```bash
git clone https://github.com/kayoMichael/chess.git
cd chess
cmake -S . -B build-release -DCMAKE_BUILD_TYPE=Release
cmake --build build-release -j
```

## Usage

### UCI Mode
The engine communicates via the Universal Chess Interface (UCI) protocol:

```bash
./chess
```

Then enter UCI commands:
```
uci
isready
position startpos
go depth 6
```
### UCI Commands
| Command | Description |
|---------|-------------|
| `uci` | Initialize UCI mode |
| `isready` | Check if engine is ready |
| `ucinewgame` | Start a new game |
| `position startpos` | Set starting position |
| `position startpos moves e2e4 e7e5` | Set position with moves |
| `position fen <fen>` | Set position from FEN string |
| `go depth <n>` | Search to depth n |
| `go movetime <ms>` | Search for specified milliseconds |
| `quit` | Exit the engine |

## Integration with a GUI
The engine works with any UCI-compatible chess GUI:
- [Arena Chess GUI](http://www.playwitharena.de/)
- [CuteChess](https://cutechess.com/)
- [Lucas Chess](https://lucaschess.pythonanywhere.com/)


### Setup
1. Build the code after cloning into a directory (E.g build-release)
2. Make the built file an executable
```bash
chmod +x build-release/chess
```
3. Add engine to your Chess GUI
- Settings → Engines → Add / Manage Engines
- Select: build-release/chess
- Set engine type: UCI
- Activate / Set as default

## Technical Details

### Board Representation
- 8x8 array-based board representation
- Piece structure with kind and color
- Incremental Zobrist hash updates in make/unmake move

### Transposition Table
- 64MB default size (~2.6 million entries)
- Stores: hash, score, depth, flag (EXACT/LOWER_BOUND/UPPER_BOUND), best move
- Replacement scheme: always replace if new depth ≥ stored depth

### Evaluation Weights
| Term | Value |
|------|-------|
| Pawn | 100 |
| Knight | 320 |
| Bishop | 330 |
| Rook | 500 |
| Queen | 900 |
| King (Checkmate) | 20000 |
| Pawn shield (per pawn) | 15 |
| Open file near king | -25 to -45 |
| Bishop blocking d/e pawn | -30 |
| Passed pawn | 10-100 (by rank) |

## Sample Game
Michael Li (1721) vs. Viktoriya Ivanovna Serebryakova (Engine, Depth 5) (1800)

0-1 | 01/25/2026 | Blitz (5 min) | C57: two knights defence, Fegatello attack, Polerio defence
```
1.e4 e5 2.Nf3 Nc6
3.Bc4 Nf6 4.Ng5 d5
5.exd5 Nxd5 6.Nxf7 Kxf7
7.Qf3+ Ke6 8.Nc3 Ncb4
9.Bb3 Bc5 10.O-O Bd4
11.a3 Bxc3 12.dxc3 Nxc2
13.Bxc2 Qf8 14.Qd3 Bd7
15.Re1 Re8 16.Bb3 c6
17.Bg5 Qf5 18.Qg3 Kf7
19.c4 Nf4 20.c5+ Nd5
21.Rad1 e4 22.Bc2 Qg4
23.Bxe4 Rxe4 24.Rxe4 Qxd1+
0-1
```

## Future Improvements

Potential enhancements to reach 2200+ ELO:
- Killer move heuristic
- History heuristic
- Null move pruning
- Late move reductions (LMR)
- Principal variation search (PVS)
- Aspiration windows
- Opening book
- Endgame tablebases
