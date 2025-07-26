# Simple VCS

Simple VCS is a lightweight, command-line version control system written in C. It allows you to track changes in your project, similar to Git, but with a much simpler set of features.

## Features

  * **Initialize Repository**: Set up a new VCS repository in your project directory.
  * **Stage Files**: Add files to a staging area, preparing them for a commit.
  * **Commit Changes**: Save the current state of your staged files as a new commit, including a unique ID and timestamp.
  * **Load Commits**: Revert your working directory to the state of a specific past commit.
  * **Blob Storage**: Stores file content as "blobs" based on their hash, avoiding duplication for identical files.

## How it Works

Simple VCS creates a `.vcs` directory in your project root to store all its data:

  * `.vcs/HEAD.txt`: Stores the ID of the next commit.
  * `.vcs/index`: A file that lists all currently staged files.
  * `.vcs/blobs/`: Directory containing "blob" files, which are the actual contents of your tracked files, named by their hash.
  * `.vcs/commits/`: Directory containing commit files, each detailing the files and their corresponding blob hashes for a specific commit.

The `fileHash` function generates a simple hash for file content. This hash is used to name the blobs and to check if a file's content has changed.

## Getting Started

### Prerequisites

  * A C compiler (e.g., GCC)

### Compilation

To compile the Simple VCS, navigate to the directory containing the source code (`main.c` if you named it that) and run:

```bash
gcc main.c -o vcs
```

This will create an executable named `vcs`.

### Basic Usage

Here are the basic commands you can use with Simple VCS:

-----

## Commands

### `vcs init`

Initializes a new Simple VCS repository in the current directory. This creates the `.vcs` directory and its necessary subdirectories and files.

```bash
./vcs init
```

### `vcs stage <file>`

Adds a file to the staging area. You can stage multiple files by running this command for each file.

```bash
./vcs stage my_document.txt
./vcs stage src/main.c
```

### `vcs commit`

Creates a new commit with all the files currently in the staging area. Each staged file's content is stored as a blob, and the commit record includes the file's name and its corresponding blob hash.

```bash
./vcs commit
```

### `vcs load <commit_id>`

Restores the files in your working directory to the state they were in at the specified `commit_id`. This will overwrite existing files with the versions from that commit.

```bash
./vcs load 0
./vcs load 1
```

-----

## Example Workflow

1.  **Initialize the repository:**

    ```bash
    ./vcs init
    ```

2.  **Create a file:**

    ```bash
    echo "Hello VCS!" > hello.txt
    ```

3.  **Stage the file:**

    ```bash
    ./vcs stage hello.txt
    ```

4.  **Commit the staged file:**

    ```bash
    ./vcs commit
    ```

5.  **Modify the file:**

    ```bash
    echo "Hello, updated VCS!" > hello.txt
    ```

6.  **Stage the modified file:**

    ```bash
    ./vcs stage hello.txt
    ```

7.  **Commit the modified file:**

    ```bash
    ./vcs commit
    ```

8.  **Load a previous commit (e.g., commit ID 0):**

    ```bash
    ./vcs load 0
    ```

    Now, `hello.txt` will revert to "Hello VCS\!".

## Limitations

This is a very basic VCS and has several limitations:

  * **No Branching or Merging**: Does not support creating branches or merging changes.
  * **Simple Hashing**: The `fileHash` function is a very basic hash and is prone to collisions, which could lead to incorrect file tracking in real-world scenarios.
  * **No File Deletion Tracking**: Does not track file deletions. If you delete a file and then load an old commit, the file might not be restored unless it was explicitly part of that commit's snapshot.
  * **Limited Error Handling**: Error handling is basic and could be improved for robustness.
  * **Fixed Buffer Size**: The `bufferSize` is a fixed value, which might not be efficient for very large or very small files.
  * **No "unstage" or "status" commands**: No way to remove files from the staging area or view the current status of files (modified, staged, etc.).

-----
