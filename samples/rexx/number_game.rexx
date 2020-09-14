/* The NUMBER GAME - User tries to guess a number between 1 and 10 */

/* Generate a random number between 1 and 10 */

the_number = random(1,10)

say "I’m thinking of number between 1 and 10. What is it?"

pull the_guess

if the_number = the_guess then
   say 'You guessed it!'
else
   say 'Sorry, my number was: ' the_number

say 'Bye!'
