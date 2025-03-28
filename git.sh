#!/bin/bash

# Script to add, commit, and push all changes to the origin repository

# Check if a commit message is provided, use a default message if not
if [ -z "$1" ]; then
    commit_message="Default commit message"
else
    commit_message="$1"
fi

# Add all changes
git add .

# Commit changes with the provided or default message
git commit -m "$commit_message"

# Push changes to the origin repository
git push origin main

echo "Changes have been pushed to the origin repository."