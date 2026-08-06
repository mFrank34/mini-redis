# Mini Redis
This project is a recreation of Redis with a corresponding key and data design, 
for quick random access. This recreation is lite remake of simlar system design to
redis with different design with less complexity.

---

## Entry Points
- GET : retrieve the stored object from a known key.
- SET : set a key and object to be retrieved later.
- DEL : delete a know key from store database.

## Features 
- handles TCP Request 
- handles request (GET, SET, DEL)
- Stores request as key and data (Key: string,data: string)
- Threaded ability for handling multi-able request
- Sharded memory design for helping threading support and read and writes speed.
- **More being built**


## File Description
- client : python test scripts and client access scripts.
- outline : project work sheet, which outlines the project scope.
- src : project source files

## Application Layout
- Main : application entry point
- Server : main application head designed to handle request and metadata.
- Store : System design to store keys and data within memory.
- Parser : small system design to break down requesting small parts for processing.
- Threads: a Threaded pool for application to break tasked down into parts across threads.
- Wal : Write-ahead log for storing keys and metadata in long term storage.

### Authors
- Michael Franks

