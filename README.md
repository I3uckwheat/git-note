Git Note
A simple program to take notes alongside the famous `git` software!


# Example 
(while on branch main)

```
$ ./git-note
| branch (notes count) : last note
----------------------------------
| main (3): hello world
$ ./git-note main
id | note
---------
 1 | hello world
 2 | hello world
 3 | tst
$ ./git-note main 1
id: 1 | Created: Thu Mar 13 15:30:31 2025 | Modified: Thu Mar 13 15:30:31 2025
----------------------------------------------------------------------------
hello world
$ ./git-note --branch testing_notes -a "hi from a test!"
$ ./git-note
| branch (notes count) : last note
----------------------------------
| testing_notes (1): hi from a test!
| main (3): hello world
$ ./git-note testing_notes
id | note
---------
 1 | hi from a test!

```

# Compiling

This should all be done from the root directory of this repo

```
mkdir build
cd build
cmake ..
make
```

This will create a binary called `git-note`, you can show the help by typing `./git-note --help`. Running this binary will create a directory in your HOME directory called `.git-note`

# Usage

```
git-note [flags] [branch] [note id]
---

Flags:
-l [branch] [note id]
    Displays a list of all notes for a repo, notes on a branch, or details about a noteid.
    (-l can be omitted for viewing)
    examples:
      'git-note -l' - Shows all notes in the repo
      'git-note -l main' - Shows all notes on main
      'git-note main 1' - Shows details about note 1 on branch main

-h 
  Shows help

-a [note] 
  add a note on the current branch. Can be used with the '--branch branchname' flag to add a note to a different branch. Omit a note to open the editor
  example:
  'git-note -a "hello world"' - Add a note on the current branch with the content "hello world"
  'git-note --branch example_branch -a' Add a note to 'example_branch' via an editor

-e [branch] [note id]
  Edit a note. Opens the default editor to make edits.
  example:
  'git-note -e main 1' - Edit the note on main with id 1

-d [branch] [note id]
  Delete a note.
  example:
  'git-note -d main 1' - Delete note on branch main, with id 1

```
