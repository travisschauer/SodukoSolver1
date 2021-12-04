// Travis Schauer 
// 10/15/20 completed 10/23/20
// Kramer 3700
// This code reads a 9x9 soduku puzzle where the input consits of integers and periods and spits out a solution

#include <iostream>
#include <cstdint>
#include "stack.h"

using namespace std;

#define IS_FILLED_IN(r,c) (((board[r][c]) & FILLED_IN_MASK) != 0)
#define MARK_INVALID(r,c,i) (board[r][c] &= (uint16_t)~(uint16_t)(VALID_START >> (uint16_t)(i)))

const uint16_t
    FILLED_IN_MASK = 0x8000,
    VALID_MASK = 0x1ff0,
    VALUE_MASK = 0x000f,
    VALID_START = 0x2000;

int countBits(uint16_t n){
    int count = 0;

    while (n != 0){
        count++;
        n &= n-1;
    }

    return count;
}

void outPut(uint16_t board[][9]){

    cout << "Solution: " << endl;
    cout << " " << endl;
    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++){
            cout << (board[i][j] & VALUE_MASK);
        }

        cout << " " << endl;
    }

}

bool FindBest(uint16_t board[][9], Stack<uint16_t> &puzzle){
    /* 
    for each empty cell board[i][j] do
        Mark all digits as valid choices
    end for*/

    for (uint16_t i = 0; i < 9; i++){
        for (uint16_t j = 0; j < 9; j++){
            if (!IS_FILLED_IN(i,j)){
                board[i][j] = VALID_MASK;
            }
        }
    }

    /*
    for each filled in cell board[i][j] do
        for each unfilled cell in row i do
            Mark digit in board[i][j] as an invalid choice
        endfor
        for each unfilled cell in column j do
            Mark digit in board[i][j] as an invalid choice
        end for
        for each unfilled cell in 3x3 block containing board[i][j] do
            Mark digit in board[i][j] as an invalid choice
        end for
    end for*/

    for (uint16_t i = 0; i < 9; i++){
        for (uint16_t j = 0; j < 9; j++){
            uint16_t choice = board[i][j] & VALUE_MASK;

            if (IS_FILLED_IN(i,j)){

                for(uint16_t c = 0; c < 9; c++){
                    if(!IS_FILLED_IN(i,c)){
                        MARK_INVALID(i,c,choice);
                    }
                }

                for(uint16_t r = 0; r < 9; r++){
                    if(!IS_FILLED_IN(r,j)){
                        MARK_INVALID(r,j,choice);
                    }
                }
                
                int r2 = (i / 3) * 3;
                int c2 = (j / 3) * 3;
                for (uint16_t r = 0; r < 3; r++){
                    for (uint16_t c = 0; c < 3; c++){
                        if(!IS_FILLED_IN(r+r2,c+c2)){
                            MARK_INVALID(r+r2,c+c2,choice);
                        }
                    }
                }


            }
        }
    }

    /*
    set low <-- 10
    for each empty cell board[i][j] do
        Count 1-bits in valid choices for board[i][j]
        if count < low then
            low <-- count
            ibest <-- i
            jbest <-- j
        end if
    end for */

    uint16_t low = 10;
    uint16_t ibest;
    uint16_t jbest;
    int count = 0;
    for (uint16_t i = 0; i < 9; i++){
        for (uint16_t j = 0; j < 9; j++){

            //Count 1-bits in valid choices for board[i][j]
            //valid choice --> IS_FILLED_IN or VALID_MASK
            if (!IS_FILLED_IN(i,j)){
                count = countBits(board[i][j] & VALID_MASK);
            

                if (count < low){
                    low = count;
                    ibest = i;
                    jbest = j;
                }
            }
        }
    }


    /*
    if low = 10 then
        return false
    end if */

    if (low == 10){
        return false;
    }

    /*
    Mark board[ibest][jbest] as filled in
    Push(ibest,jbest) onto the stack */

    board[ibest][jbest] |= FILLED_IN_MASK;
    // row = e/9 column = e % 9
    uint16_t e = ibest*9+jbest;
    puzzle.push(e);


    return true;
}

bool findNextChoice(uint16_t &cell) {
    // loop from 1 to 9
      // if (((VALID_START >> loop counter) & cell) != 0)
        // mask off VALUE_MASK bits from cell
        // cell |= loop counter
        // mask off VALID_START >> loop counter from cell
        // return true
    // if the loop falls out and you get here, no choices are left, return false

    for (int i = 1; i <= 9; i++){
        if (((VALID_START >> i) & cell) != 0){
            cell &= ~VALUE_MASK;
            cell |= i;
            cell &= ~(VALID_START >> i); // cell = cell & ....
            return true;
        }
    }

    return false;
}


void solve(uint16_t board[][9], Stack<uint16_t> &puzzle){
    /* Select the best empty cell and place its location on the stack

    while true do
        Let (i,j) be the location on top of the stack
        
        Select the next valid choice for board[i][j]
        if no such choice exists then
            Mark board[i][j] as not filled in
            Pop the stack
            if the stack is empty then
                return; the puzzle has no solutions
            end if
            continue
        end if

        Select the best empty cell and place its location on the stack
        if no such cell exists then
            break
        end if
    end while
    
    Output solution */    

    FindBest(board, puzzle);

    while (true){
        uint16_t row, column;
        uint16_t e = puzzle.peek();

        row = e/9;
        column = e % 9;

        if (!findNextChoice(board[row][column])) {
            board[row][column] &= ~FILLED_IN_MASK;

            puzzle.pop();
            if(puzzle.isEmpty()){
                cout << "Puzzle has no solutions" << endl;
                return;
            }
            
            continue;
        }


        if (!FindBest(board, puzzle))
            break;
    }

    outPut(board);
}

int main(){
    uint16_t board[9][9];
    Stack<uint16_t> puzzle;
    char n;

    for(int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++){
            cin >> n;

            if (n == '.'){
                board[i][j] = 0;
            } else {
                board[i][j] = FILLED_IN_MASK | (n - '0');
            }
        }
    }

    solve(board, puzzle);

    return 0;
}