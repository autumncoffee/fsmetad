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
$ curl -X POST 'http://127.0.0.1:15464/v1/file/new/' -H 'Content-Type: application/json' -d '{"name": "some name", "path": "/full/path/to/existing/file/on/disk", "ctype": "text/plain", "tags": []}'
{"id":"18ae980400000000d8e8eb5c000000000000"}
```

Downloading file:

```
$ curl 'http://127.0.0.1:15464/v1/file/get/18ae980400000000d8e8eb5c000000000000' -o file
```

Tagging a file:

```
$ curl -X POST 'http://127.0.0.1:15464/v1/file/tag/18ae980400000000d8e8eb5c000000000000' -H 'Content-Type: application/json' -d '{"tags": ["asd", "qwe", "zxc"]}'
```

Selecting last 20 files tagged with `asd`:

```
$ curl 'http://127.0.0.1:15464/v1/file/tag/asd'
{"results":[{"id":"18ae980400000000d8e8eb5c000000000000"}]}
```

Selecting 20 files older than `18ae980400000000d8e8eb5c000000000000` tagged with `asd`:

```
$ curl 'http://127.0.0.1:15464/v1/file/tag/asd/18ae980400000000d8e8eb5c000000000000'
{"results":[]}
```

Removing `asd` tag from `18ae980400000000d8e8eb5c000000000000`:

```
$ curl -X POST 'http://127.0.0.1:15464/v1/file/untag/18ae980400000000d8e8eb5c000000000000/asd'
```

Getting file info:

```
$ curl 'http://127.0.0.1:15464/v1/file/info/18ae980400000000d8e8eb5c000000000000'
{"ctype":"text/plain","id":"18ae980400000000d8e8eb5c000000000000","name":"some name","size":1040,"tags":["qwe", "zxc"]}
```

Removing file (also removes file from disk!):

```
$ curl -X POST 'http://127.0.0.1:15464/v1/file/remove/18ae980400000000d8e8eb5c000000000000'
```
