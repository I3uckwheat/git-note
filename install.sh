#!/bin/bash

INSTALL_DIR="$HOME/.bin"
echo "This script will install git-note to $INSTALL_DIR"
read -p "Are you sure? [Yy]" -n 1 -r
if [[ $REPLY =~ [Yy]$ ]]
then
    cd build
    cmake ..
    make
    mkdir --parents $INSTALL_DIR
    mv ./git-note $INSTALL_DIR/git-note
    echo "Add '\$HOME/.bin/' to your bashrc/zshrc/fshrc/etc file to use git-note"
fi
