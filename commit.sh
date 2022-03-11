#! /bin/zsh

git fetch
git pull origin Lab1
git add .
git commit -am "$*  $(date)"
git push origin main
