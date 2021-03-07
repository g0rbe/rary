# Rary

## Install

### Build

Create both shared object (`/usr/lib/x86_64-linux-gnu/library.so`) and archive file (`/usr/lib/x86_64-linux-gnu/library.a`),
and install the man pages.

```bash
sudo make install
sudo make clean
```

### Man pages

TO install man pages only:

```bash
sudo make man
```

## Uninstall

```bash
sudo make remove
```

## Include

### Shared object

Include the library with `-lrary`.

### Archive

Add the archive file to `gcc`:

```bash
gcc text.c /usr/lib/x86_64-linux-gnu/library.a
```

## Documentation

Man pages:
- `rary.h`
- `RARY_*`