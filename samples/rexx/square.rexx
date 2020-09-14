/* SQUARE:                                                              */
/*                                                                      */
/*    Squares a number by calling an internal subroutine                */

arg number_in .                /* retrieve the command-line argument    */

call square_the_number number_in
say 'You entered:' number_in '  Squared it is:' result

exit 0

/* SQUARE_THE_NUMBER:                                                   */
/*                                                                      */
/*    Squares the number and RETURNs it into RESULT                     */

square_the_number: procedure

   arg the_number
   return  the_number * the_number
