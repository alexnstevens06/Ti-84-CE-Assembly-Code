# Ti-84-CE-Assembly-Code

`snake/` is a new Snake game for the TI-84 Plus CE. It was written for this repository and built with the CE C toolchain, CEdev 15.0 (`ez80-clang`). It is not older recovered source.

Arrow keys move the snake. Eating the food grows it and raises the score. The game ends on a wall or on the snake's own body. Clear quits. After a loss, or if the board is full, Enter starts again.

`ti84 assembly code.zip` is the earlier material that was already in the repo.

## Build

Install CEdev 15.0 and put its `bin` directory on `PATH` (`cedev-config` and `ez80-clang`). From `snake/`:

```sh
make
```

That writes `snake/bin/SNAKE.8xp`.
