fsmetad
===

Simple file storage API: manages metadata, serves files.

Building
---

```
$ nix-build
```

Running
---

```
$ DB_PATH=/path/to/metadata/database/dir BIND_V4=127.0.0.1 BIND_PORT=15464 ./result/bin/fsmetad
```

Using
---

Importing new file:

```
$ curl -X POST 'http://127.0.0.1:15464/v1/file/new/' -H 'Content-Type: application/json' -d '{"name": "some name", "path": "/full/path/to/existing/file/on/disk", "ctype": "text/plain"}'
{"id":"18ae980400000000d8e8eb5c000000000000"}
```

Downloading file:

```
$ curl 'http://127.0.0.1:15464/v1/file/get/18ae980400000000d8e8eb5c000000000000' -o file
```