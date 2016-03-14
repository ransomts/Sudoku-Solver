#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

bool finished(int puzzle[9][9]){

   for (int i = 0; i < 9; i++) {
      for (int j = 0; j < 9; j++) {

	 if (puzzle[i][j] == 0) {
	    return false;
	 }
      }
   }
   return true;
}

bool can_go_in_rc(int puzzle[9][9], int value, int row, int col) {

   // check if something is already there
   if (puzzle[row][col] != 0) {
      // printf("value already at (%d, %d)\n", row, col);
      return false;
   }

   // check if the number is already in the row
   for (int i = 0; i < 9; i++) {
      if (puzzle[row][i] == value) {
	 // printf("%d in row at (%d, %d)\n", value, row, i);
	 return false;
      }
   }

   // check if the number is already in the column
   for (int j = 0; j < 9; j++) {
      if (puzzle[j][col] == value) {
	 // printf("%d in column at (%d, %d)\n", value, j, col);
	 return false;
      }
   }

   // check the box for the value
   int box_row = row / 3;
   int box_col = col / 3;
   for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
	 if (puzzle[box_row * 3 + i][box_col * 3 + j] == value) {
	    // printf("%d already in box at (%d, %d)\n", value, box_row*3+i, box_col*3+j);
	    return false; 
	 }
      }
   }
   
   // printf("can put %d in (%d, %d)\n", value, row, col);
   return true;
}

int should_go_in_spot(int puzzle[9][9], int value, int row, int col) {

   // if it can't go there, then don't put it there!
   if (can_go_in_rc(puzzle, value, row, col) == false) {
      return 0;
   }
   
   // see if its the last number in a row
   int zero_counter = 0;
   int zero_row = 0;
   for (int i = 0; i < 9; i++) {
      if (puzzle[i][col] == 0) {
	 zero_counter++;
	 zero_row = i;
      }
   }
   // if there is only one zero in the row, and it's on this row
   if (zero_counter == 1 && zero_row == row) {
      // printf("only zero in column at (%d, %d)\n", row, col);
      return 1;
   }

   zero_counter = 0;
   int zero_col = 0;
   for (int j = 0; j < 9; j++) {
      if (puzzle[row][j] == 0) {
	 zero_counter++;
	 zero_col = j;
      } 
   }
   // if there is only one zero in the column, and it's on this column
   if (zero_counter == 1 && zero_col == col) {
      // printf("only zero in row at (%d, %d)\n", row, col);
      return 2;
   }

   // check if it's the last zero in the box
   zero_counter = 0;
   int box_row = row / 3;
   int box_col = col / 3;
   for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
	 if (puzzle[box_row * 3 + i][box_col * 3 + j] == 0) {
	    zero_col = box_col * 3 + i;
	    zero_row = box_row * 3 + j;
	    zero_counter++;
	 }
	 //// printf("%d ", puzzle[box_row * 3 + i][box_col * 3 + j]);
      }
      //// printf("\n");
   }
   if (zero_counter == 1 && zero_row == row && zero_col == col) {
      // printf("only zero in box at (%d, %d)\n", row, col);
      return 3;
   } else {
      // printf("zero counter: %d\n", zero_counter);
      // printf("zero row: %d\n", zero_row);
      // printf("zero col: %d\n", zero_col);
   }
   
   return 0;
}

bool value_in_row(int puzzle[9][9], int value, int row) {
   if (row < 0 || row > 8) {
      return false;
   }
   for (int i = 0; i < 9; i++) {
      if (puzzle[row][i] == value) { return true; }
   }
   return false;
}

bool value_in_col(int puzzle[9][9], int value, int col) {
   
   if (col < 0 || col > 8) {
      return false;
   }
   for (int i = 0; i < 9; i++) {
      if (puzzle[i][col] == value) { return true; }
   }
   return false;
}

bool only_zero_in_box_row(int puzzle[9][9], int row, int col) {

   if (puzzle[row][col] != 0) { return false; }
   
   switch (row % 3) {
   case 0: 
      return puzzle[row + 1][col] != 0 && puzzle[row + 2][col] != 0;
   case 1: 
      return puzzle[row + 1][col] != 0 && puzzle[row - 1][col] != 0;
   default:
      return puzzle[row - 1][col] != 0 && puzzle[row - 2][col] != 0;
   }
}      

bool only_zero_in_box_col(int puzzle[9][9], int row, int col) {

   if (puzzle[row][col] != 0) { return false; }
   
   switch (col % 3) {
   case 0: 
      return puzzle[row][col + 1] != 0 && puzzle[row][col + 2] != 0;
   case 1: 
      return puzzle[row][col + 1] != 0 && puzzle[row][col - 1] != 0;
   default:
      return puzzle[row][col - 1] != 0 && puzzle[row][col - 2] != 0;
   }
}      

// if the number is in the two other rows/columns and
//   there is only one place left to put it, put it there
bool two_side_squeeze(int puzzle[9][9], int value, int row, int col) {

   int rel_box_row = row % 3;
   int rel_box_col = col % 3;
   bool value_in_row_above = value_in_row(puzzle, value, row - 1);
   bool value_in_row_below = value_in_row(puzzle, value, row + 1);
   bool value_in_row_two_below = value_in_row(puzzle, value, row - 2);
   bool value_in_row_two_above = value_in_row(puzzle, value, row + 2);

   bool value_in_col_left = value_in_col(puzzle, value, col - 1);
   bool value_in_col_right = value_in_col(puzzle, value, col + 1);
   bool value_in_col_two_left = value_in_col(puzzle, value, col - 2);
   bool value_in_col_two_right = value_in_col(puzzle, value, col + 2);

   if (only_zero_in_box_col(puzzle, row, col) == false) { return false; }
   
   if (rel_box_row == 0) {
      if (value_in_row_below && value_in_row_two_below) { return true; }
   } else if (rel_box_row == 1) {
      if (value_in_row_above && value_in_row_below) { return true; }	  
   } else {
      if (value_in_row_above && value_in_row_two_above) { return true; }
   }

   
   if (only_zero_in_box_row(puzzle, row, col) == false) { return false; }
   
   if (rel_box_col == 0) {
      if (value_in_col_right && value_in_col_two_right) { return true; }
   } else if (rel_box_col == 1) {
      if (value_in_col_left && value_in_col_right) { return true; }	  
   } else {
      if (value_in_col_left && value_in_col_two_left) { return true; }
   }
   
   return false;
}

// when two rows/cols and one of the other leave only one spot 
bool three_side_squeeze(int puzzle[9][9], int value, int row, int col) {

   int rel_box_row = row % 3;
   int rel_box_col = col % 3;
   bool in_row_above = value_in_row(puzzle, value, row - 1);
   bool in_row_below = value_in_row(puzzle, value, row + 1);
   bool in_row_two_below = value_in_row(puzzle, value, row - 2);
   bool in_row_two_above = value_in_row(puzzle, value, row + 2);

   bool in_col_left = value_in_col(puzzle, value, col - 1);
   bool in_col_right = value_in_col(puzzle, value, col + 1);
   bool in_col_two_left = value_in_col(puzzle, value, col - 2);
   bool in_col_two_right = value_in_col(puzzle, value, col + 2);

   
   return false;
}

// if both adjacent rows and both adjacent columns say the number
//   should be put in the spot.
bool four_side_squeeze(int puzzle[9][9], int value, int row, int col) {

   int rel_box_row = row % 3;
   int rel_box_col = col % 3;
   /* alternate naming convention?
     a b c
     d e f
     g h i
   */

   bool value_in_row_above = value_in_row(puzzle, value, row - 1);
   bool value_in_row_below = value_in_row(puzzle, value, row + 1);
   bool value_in_row_two_below = value_in_row(puzzle, value, row - 2);
   bool value_in_row_two_above = value_in_row(puzzle, value, row + 2);

   bool value_in_col_left = value_in_col(puzzle, value, col - 1);
   bool value_in_col_right = value_in_col(puzzle, value, col + 1);
   bool value_in_col_two_left = value_in_col(puzzle, value, col - 2);
   bool value_in_col_two_right = value_in_col(puzzle, value, col + 2);
   printf("%d %d %d %d\n",
	  value_in_col_right, value_in_col_two_right,
	  value_in_row_below, value_in_row_above);

   if (rel_box_row == 0) {
      if (rel_box_col == 0) {
	 return value_in_col_right && value_in_col_two_right &&
	    value_in_row_two_below && value_in_row_two_below;
      } else if (rel_box_col == 1) {
	 return value_in_row_below && value_in_row_two_below &&
	    value_in_col_right && value_in_col_left;
      } else {
	 return value_in_col_two_left && value_in_col_left &&
	    value_in_row_below && value_in_row_two_below; }
   }
   if (rel_box_row == 1) {
      if (rel_box_col == 0) {
	 return value_in_col_right && value_in_col_two_right &&
	    value_in_row_below && value_in_row_above;
      } else if (rel_box_col == 1) {
	 return value_in_col_right && value_in_col_left &&
	    value_in_row_above && value_in_row_below;
      } else {
	 return value_in_col_two_left && value_in_col_left &&
	    value_in_row_below && value_in_row_above; }
   }
   if (rel_box_row == 2) {
      if (rel_box_col == 0) {
	 return value_in_col_right && value_in_col_two_right &&
	    value_in_row_two_above && value_in_row_above;
      } else if (rel_box_col == 1) {
	 return value_in_col_right && value_in_col_left &&
	    value_in_row_above && value_in_row_two_above;
      } else {
	 return value_in_col_two_left && value_in_col_left &&
	    value_in_row_above && value_in_row_two_above; }
   }

   return false;
}

void fill_row_col(int puzzle[9][9]) {

   // try all the numbers
   for (int value = 1; value <= 9; value++) {
      for (int row = 0; row < 9; row++) {
	 for (int col = 0; col < 9; col++) {

	    if (should_go_in_spot(puzzle, value, row, col) ||
		should_put_in_spot(puzzle, value, row, col)){
	       printf("put %d at (%d, %d)\n", value, row, col);
	       puzzle[row][col] = value;
	    }
	 }
      }
   }
}

void print_puzzle(int puzzle[9][9]) {

   for (int row = 0; row < 9; row++) {
      for (int col = 0; col < 9; col++) {
	 printf("%d ", puzzle[row][col]);
	 if (col % 3 == 2) { printf(" "); }
      }
      if (row % 3 == 2) { printf("\n"); }
      printf("\n");
   }
   printf("-------------------\n");

}

int main(){
   
   // fill in the values, with 0
   // for the unknown values

   // put a 6 at (1, 3)
     int puzzle[9][9] = {
	0,0,6, 0,0,7, 3,0,0,
	0,1,8, 0,0,9, 0,5,0,
	5,0,0, 0,0,0, 0,6,4,

     	9,2,0, 0,8,0, 0,0,0,
	0,0,0, 7,6,3, 0,0,0,
	0,0,0, 0,9,0, 0,7,5,

     	6,3,0, 0,0,0, 0,0,8,
	0,9,0, 3,0,0, 5,2,0,
	0,0,2, 4,0,0, 6,0,0
};
	
   /*
   int puzzle[9][9] = {
      // 2,9,5, 7,4,3, 8,6,1,
      0,9,5, 0,4,3, 8,6,1,
      4,3,1, 8,0,5, 9,2,7,
      8,7,6, 1,9,2, 5,4,3,
      
      0,8,7, 4,5,9, 2,1,6,
      6,1,2, 3,8,7, 4,9,5,
      5,4,9, 2,1,6, 7,3,8,

      7,6,3, 5,3,4, 1,8,9,
      9,2,8, 6,7,1, 3,5,4,
      1,5,4, 9,3,8, 6,7,2};
   */
   // Prints out the initial puzzle
   print_puzzle(puzzle);

   printf("%d\n", should_put_in_spot(puzzle, 6, 1, 3));
   /*
   while (!finished(puzzle)){
      fill_row_col(puzzle);
      print_puzzle(puzzle);
      sleep(3);
   }
   */
   // Puzzle is solved by this point
   print_puzzle(puzzle);

   return EXIT_SUCCESS;
}
