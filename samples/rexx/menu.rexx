/* MENU:                                                            */
/*                                                                  */
/*    This program display a menu and performs updates based        */
/*    on the transaction the user selects.                          */

'clear'                         /* clear the screen (Windows only)  */
tran_type = ''
do while tran_type \= 'X'       /* do until user enters ‘X’         */
   say
   say 'Select the transation type by abbreviation:'
   say
   say ' Insert = I '
   say ' Update = U '
   say ' Delete = D '
   say ' Exit = X '
   say
   say 'Your choice => '
   pull tran_type .
   select
      when tran_type = 'I' then
         call insert_routine
      when tran_type = 'U' then
         call update_routine
      when tran_type = 'D' then
         call delete_routine
      when tran_type = 'X' then do
         say
         say 'Bye!'
         end
      otherwise
         say
         say 'You entered invalid transaction type:' tran_type
         say 'Press <ENTER> to reenter the transaction type.'
         pull .
      end
end
exit 0

/* INSERT_ROUTINE goes here */
INSERT_ROUTINE: procedure
   say 'Insert Routine was executed'
   return 0

/* UDPATE_ROUTINE goes here */
UPDATE_ROUTINE: procedure
   say 'Update Routine was executed'
   return 0

/* DELETE_ROUTINE goes here */
DELETE_ROUTINE: procedure
   say 'Delete Routine was executed'
   return 0
