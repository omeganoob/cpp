#!/bin/bash

# Ask for the user's name
echo "Hello! What is your name?"
read name

# Greet the user
echo "Nice to meet you, $name!"

# Countdown from 5 to 1
echo "Let's count down from 5 to 1:"
for i in {5..1}; do
    echo "$i"
    sleep 1  # Wait for 1 second
done

# Say goodbye
echo "Goodbye, $name! Have a great day!"
