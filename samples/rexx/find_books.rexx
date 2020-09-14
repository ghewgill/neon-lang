/*  FIND BOOKS:                                                      */
/*                                                                   */
/*     This program illustrates basic arrays by retrieving book      */
/*     titles based on keyword weightings.                           */

keyword. = ''         /* initialize both arrays to all null strings  */
title. = ''

/* the array of keywords to search for among the book descriptors    */

keyword.1 = 'earth'   ;   keyword.2 = 'computers'
keyword.3 = 'life'    ;   keyword.4 = 'environment'

/* the array of book titles, each having 3 descriptors               */

title.1 = 'Saving Planet Earth'
  title.1.1 = 'earth'
  title.1.2 = 'environment'
  title.1.3 = 'life'
title.2 = 'Computer Lifeforms'
  title.2.1 = 'life'
  title.2.2 = 'computers'
  title.2.3 = 'intelligence'
title.3 = 'Algorithmic Insanity'
  title.3.1 = 'computers'
  title.3.2 = 'algorithms'
  title.3.3 = 'programming'

arg weight . /* get number keyword matches required for retrieval    */

say 'For weight of' weight 'retrieved titles are:'  /* output header */

do j=1 while title.j <> ''                   /* look at each book    */
   count = 0
   do k=1 while keyword.k <> ''              /* inspect its keywords */
      do l=1 while title.j.l <> ''
         if  keyword.k = title.j.l  then count = count + 1
      end
   end

   if count >= weight then   /* display titles matching the criteria */
      say title.j
end
