#include<iostream>
#include<fstream>
#include<string>
using namespace std;

class grid{
public:
    int value;
    int prev;
    int next;
public:
    grid(){
        set_empty();
    }

    void set_empty(){
        value = 0;
        prev = -2;
        next = -2; 
    }

    void set_filled(int p, int n){
        value = 1;
        prev = p;
        next = n;
    }

    grid &operator=(const grid& other){
        value = other.value;
        prev = other.prev;
        next = other.next;
        return *this;
    }
};

class stripe{
public:
    grid *arr;
    int head;
    int end;
private:
    int size;
public:
    stripe(int sz){
        arr = new grid[sz];
        head = -1;
        end = -1;
        size = sz;
    }

    void insert(int pos){
        int cur;
        if(pos >= size || pos < 0) {
            printf("Error: insert to a non-existed position!\n");
            exit(1);
        }
        else if(arr[pos].value == 1) {
            printf("Error: insert to a filled position\n");
            exit(2);
        }
        else{
            if(head == -1){
                arr[pos].set_filled(-1,-1);
                head = pos;
                end = pos;
            }
            else if(pos < head){
                arr[pos].set_filled(-1,head);
                arr[head].prev = pos;
                head = pos;
            }
            else if(pos > end){
                arr[pos].set_filled(end,-1);
                arr[end].next = pos;
                end = pos;
            }
            else{
                if(pos > size/2){
                    cur = end;
                    while(pos < cur) cur = arr[cur].prev;
                    arr[pos].set_filled(cur,arr[cur].next);
                    arr[cur].next = pos;
                    arr[arr[pos].next].prev = pos;
                }
                else{
                    cur = head;
                    while(pos > cur) cur = arr[cur].next;
                    arr[pos].set_filled(arr[cur].prev,cur);
                    arr[cur].prev = pos;
                    arr[arr[pos].prev].next = pos;
                }
            }
        }
    }

    void erase(int pos){
        int p_next, p_prev;
        if(pos >= size || pos < 0) {
            printf("Error: erase to a non-existed position!\n");
            exit(3);
        }
        else if(arr[pos].value == 0) {
            printf("Error: erase to a empty position\n");
            exit(4);
        }
        if(head == end){
            arr[pos].set_empty();
            head = -1;
            end = -1;
        }
        else{
            //* 曾經奇怪的狀況
            p_next = arr[pos].next;
            p_prev = arr[pos].prev;
            if(p_next > -1) arr[p_next].prev = arr[pos].prev + 1;
            // printf("size1=%d\n", size);
            if(p_prev > -1) arr[p_prev].next = arr[pos].next - 1;
            // printf("size2=%d, p_prev=%d, p_prev_next=%d\n", size, p_prev, arr[p_prev].next);
            if(head == pos){
                head = arr[pos].next;
                if(head != -1) arr[head].prev = -1;
            }
            if(end == pos){
                end = arr[pos].prev;
                if(end != -1) arr[end].next = -1;
            }
            // printf("before:");
            // print_out();
            sink(pos);
            // printf("after :");
            // print_out();
            // printf("\n");
        }
    }

    void sink(int pos){
        int cur = pos;
        while(cur > 0 && cur > head){
            if(arr[cur-1].value == 1){
                arr[cur] = arr[cur-1];
                if(arr[cur].next != -1)arr[cur].next++;
                if(arr[cur].prev != -1) arr[cur].prev++;
            }
            else{
                arr[cur].set_empty();
            }
            cur--;
            // printf("cur=%d :",cur);
            // print_out();
        }
        arr[cur].set_empty();
        if(cur == head) head++; //!為啥是大於阿
        if(pos > end) end++;
    }

    int find_the_least_greater(int pos){
        int cur = head;
        while(cur < pos && cur != -1) cur = arr[cur].next;
        if(cur == -1) return size;
        else return cur;
    }

    int get_ith(int i, const char* from){
        if(i<0 || i>=size) {
            printf("Error: take a non-existed grid in stripe, size=%d, i=%d, from:%s\n", size, i, from);
            exit(5);
        }
        return arr[i].value;
    }

    void print_out(){
        printf("#head=%d, end=%d, ", head, end);
        for(int i=0;i<size;i++){
            printf("%d ", arr[i].value);
        }
        printf("\n");
    }
    void print_out2(int c=-1){
        printf("#head=%d, end=%d\n", head, end);
        for(int i=0;i<size;i++){
            if(c==-1) printf("#%d, next=%d, prev=%d\n", arr[i].value, arr[i].next, arr[i].prev);
            else printf("#[col=%2d,row=%2d]:%d, next=%d, prev=%d\n", c, i, arr[i].value, arr[i].next, arr[i].prev);
        }
        printf("\n");
    }
};

class tetrix_map{
public:
    stripe **col_maj;
    int rows;
    int cols;
    int *row_number;

    tetrix_map(int r, int c){
        rows = r;
        cols = c;
        row_number = new int[rows];
        col_maj = new stripe*[cols]; 
        for(int i=0;i<cols;i++){
            col_maj[i] = new stripe(rows);
        }
    }

    bool check_bingo(){ //extension
        bool change = false;
        for(int i=4;i<rows;i++) row_number[i] = 0;
        for(int i=0;i<cols;i++){
            for(int j=4;j<rows;j++){
                row_number[j] += col_maj[i]->get_ith(j,"check bingo");
            }
        }
        for(int i=4;i<rows;i++){
            if(row_number[i] == cols){
                // printf("##erase row %d\n", i);
                // print_out();
                change = true;
                erase_row(i);
            }
        }
        return change;
    }

    bool four_bingo(){ //extension
        bool change = false;
        for(int i=4;i<8 && i<rows;i++) row_number[i] = 0;
        for(int i=0;i<cols;i++){
            for(int j=4;j<8 && j<rows;j++){
                row_number[j] += col_maj[i]->get_ith(j,"four bingo");
            }
        }
        for(int i=4;i<8 && i<rows;i++){
            if(row_number[i] == cols){
                change = true;
                erase_row(i);
            }
        }
        return change;
    }

    void erase_row(int r){
        grid *carr;
        int cur;
        for(int i=0;i<cols;i++){
            col_maj[i]->erase(r);
        }
    }

    string print_out(){
        string out;
        for(int i=4;i<rows;i++){ //!extension
            for(int j=0;j<cols-1;j++){
                // printf("%c ",(col_maj[j]->get_ith(i,"tetrix debug1")==1)? 'X':'.');
                out.append(1,(col_maj[j]->get_ith(i,"tetrix printout1") == 1)?'1':'0');
                out.append(1,' ');
            }
            // printf("%c\n",(col_maj[cols-1]->get_ith(i,"tetrix debug2")==1)? 'X':'.');
            out.append(1,(col_maj[cols-1]->get_ith(i,"tetrix printout2") == 1)?'1':'0');
            out.append(1,'\n');
        }
        // cout << out << endl;
        return out;
    }

    void detail_print(){
        for(int i=0;i<cols;i++){
            col_maj[i]->print_out2(i);
        }
    }

    bool is_death(){
        for(int i=0;i<cols;i++){
            if(col_maj[i]->head < 4 && col_maj[i]->head > -1) return true;
        }
        return false;
    }
};
tetrix_map *tetrixmap;

class square{
private:
    int row, col;
public:
    square(){
        row = -1;
        col = -1;
    }
    square(int r, int c){
        set_position(r,c);
    }
    void set_position(int r, int c){
        row = r;
        col = c;
    }
    void move(int dir, int step){
        if(dir == 0) row += step;
        else col += step;
    }
    int ground_distance(){
        return tetrixmap->col_maj[col]->find_the_least_greater(row) - row - 1;
    }
    int left_distance(){
        int cur = col - 1;
        if(row < 0){
            return col - 1;
        }
        while(cur > -1 && tetrixmap->col_maj[cur]->get_ith(row,"left distance") == 0) cur--;
        return col - cur - 1; 
    }
    int right_distance(){
        int cur = col + 1;
        if(row < 0){
            return tetrixmap->cols - col - 1;
        }
        else{
            while(cur < tetrixmap->cols && tetrixmap->col_maj[cur]->get_ith(row,"left distance") == 0) cur++;
            return cur - col -1;
        }
    }
    int get_row(){
        return row;
    }
    int get_col(){
        return col;
    }
};


class block{
public:
    square squares[4];
    int ref_row;
    int ref_col;
    int ground_step;

    block(){
        ref_row = -2;
        ref_col = -1;
        ground_step = 0;
    }

    block(const block& other){

    }

    block(char* type, int pos, int gstep){
        ground_step = gstep;
        ref_row = -1;
        ref_col = pos;
        square_generate(type);
    }

    void set(char* type, int pos, int gstep){
        ground_step = gstep;
        ref_row = -1;
        ref_col = pos;
        square_generate(type);
    }

    void square_generate(char* &type){
        switch(type[0]){
            case 'T':
                switch(type[1]){
                    case '1':
                        squares[0].set_position(ref_row,ref_col+1);
                        squares[1].set_position(ref_row-1,ref_col);
                        squares[2].set_position(ref_row-1,ref_col+1);
                        squares[3].set_position(ref_row-1,ref_col+2);
                    break;
                    case '2':
                        squares[0].set_position(ref_row,ref_col+1);
                        squares[1].set_position(ref_row-1,ref_col);
                        squares[2].set_position(ref_row-1,ref_col+1);
                        squares[3].set_position(ref_row-2,ref_col+1);
                    break;
                    case '3':
                        squares[0].set_position(ref_row,ref_col);
                        squares[1].set_position(ref_row,ref_col+1);
                        squares[2].set_position(ref_row,ref_col+2);
                        squares[3].set_position(ref_row-1,ref_col+1);
                    break;
                    case '4':
                        squares[0].set_position(ref_row,ref_col);
                        squares[1].set_position(ref_row-1,ref_col);
                        squares[2].set_position(ref_row-1,ref_col+1);
                        squares[3].set_position(ref_row-2,ref_col);
                    break;
                }
            break;
            case 'L':
                switch(type[1]){
                    case '1':
                        squares[0].set_position(ref_row,ref_col);
                        squares[1].set_position(ref_row,ref_col+1);
                        squares[2].set_position(ref_row-1,ref_col);
                        squares[3].set_position(ref_row-2,ref_col);
                    break;
                    case '2':
                        squares[0].set_position(ref_row,ref_col);
                        squares[1].set_position(ref_row-1,ref_col);
                        squares[2].set_position(ref_row-1,ref_col+1);
                        squares[3].set_position(ref_row-1,ref_col+2);
                    break;
                    case '3':
                        squares[0].set_position(ref_row,ref_col+1);
                        squares[1].set_position(ref_row-1,ref_col+1);
                        squares[2].set_position(ref_row-2,ref_col);
                        squares[3].set_position(ref_row-2,ref_col+1);
                    break;
                    case '4':
                        squares[0].set_position(ref_row,ref_col);
                        squares[1].set_position(ref_row,ref_col+1);
                        squares[2].set_position(ref_row,ref_col+2);
                        squares[3].set_position(ref_row-1,ref_col+2);
                    break;
                }
            break;
            case 'J':
                switch(type[1]){
                    case '1':
                        squares[0].set_position(ref_row,ref_col);
                        squares[1].set_position(ref_row,ref_col+1);
                        squares[2].set_position(ref_row-1,ref_col+1);
                        squares[3].set_position(ref_row-2,ref_col+1);
                    break;
                    case '2':
                        squares[0].set_position(ref_row,ref_col);
                        squares[1].set_position(ref_row,ref_col+1);
                        squares[2].set_position(ref_row,ref_col+2);
                        squares[3].set_position(ref_row-1,ref_col);
                    break;
                    case '3':
                        squares[0].set_position(ref_row,ref_col);
                        squares[1].set_position(ref_row-1,ref_col);
                        squares[2].set_position(ref_row-2,ref_col);
                        squares[3].set_position(ref_row-2,ref_col+1);
                    break;
                    case '4':
                        squares[0].set_position(ref_row,ref_col+2);
                        squares[1].set_position(ref_row-1,ref_col);
                        squares[2].set_position(ref_row-1,ref_col+1);
                        squares[3].set_position(ref_row-1,ref_col+2);
                    break;
                }
            break;
            case 'S':
                if(type[1] == '1'){
                    squares[0].set_position(ref_row,ref_col);
                    squares[1].set_position(ref_row,ref_col+1);
                    squares[2].set_position(ref_row-1,ref_col+1);
                    squares[3].set_position(ref_row-1,ref_col+2);
                }
                else{
                    squares[0].set_position(ref_row,ref_col+1);
                    squares[1].set_position(ref_row-1,ref_col);
                    squares[2].set_position(ref_row-1,ref_col+1);
                    squares[3].set_position(ref_row-2,ref_col);
                }
            break;
            case 'Z':
                if(type[1] == '1'){
                    squares[0].set_position(ref_row,ref_col+1);
                    squares[1].set_position(ref_row,ref_col+2);
                    squares[2].set_position(ref_row-1,ref_col);
                    squares[3].set_position(ref_row-1,ref_col+1);
                }
                else{
                    squares[0].set_position(ref_row,ref_col);
                    squares[1].set_position(ref_row-1,ref_col);
                    squares[2].set_position(ref_row-1,ref_col+1);
                    squares[3].set_position(ref_row-2,ref_col+1);
                }
            break;
            case 'I':
                if(type[1] == '1'){
                    squares[0].set_position(ref_row,ref_col);
                    squares[1].set_position(ref_row-1,ref_col);
                    squares[2].set_position(ref_row-2,ref_col);
                    squares[3].set_position(ref_row-3,ref_col);
                }
                else{
                    squares[0].set_position(ref_row,ref_col);
                    squares[1].set_position(ref_row,ref_col+1);
                    squares[2].set_position(ref_row,ref_col+2);
                    squares[3].set_position(ref_row,ref_col+3);
                }
            break;
            case 'O':
                squares[0].set_position(ref_row,ref_col);
                squares[1].set_position(ref_row,ref_col+1);
                squares[2].set_position(ref_row-1,ref_col);
                squares[3].set_position(ref_row-1,ref_col+1);
            break;
        }
    }

    void move(int dir, int step){
        for(int i=0;i<4;i++){
            squares[i].move(dir,step);
        }
        if(dir == 0) ref_row += step;
        else ref_col += step;
    }

    int look_down(){
        int minimum = squares[0].ground_distance();
        int temp;
        for(int i=1;i<4;i++){
            temp = squares[i].ground_distance();
            if(minimum > temp) minimum = temp;
        }
        return minimum;
    }

    bool drop(){
        int minimum = look_down();
        // printf("#minimum=%d\n", minimum);
        move(0,minimum);
        return (minimum != 0)? false : true;
    }

    bool crab(int step){
        int minimum = 100;
        int temp;
        if(step > 0){
            for(int i=0;i<4;i++){
                temp = squares[i].right_distance();
                if(minimum > temp) minimum = temp;
            }
            if(step > minimum){
                printf("Error: invalid crab\n");
                exit(6);
            }
            move(1,step);
        }
        else{
            step *= -1;
            for(int i=0;i<4;i++){
                temp = squares[i].left_distance();
                if(minimum > temp) minimum = temp;
            }
            if(step > minimum) {
                printf("Error: invalid crab\n");
                exit(7);
            }
            move(1,-1 * step);
        }
        return (look_down() == 0)? true : false;
    }
    void stick_on_map(){
        for(int i=0;i<4;i++){
            // printf("#square[%d]:(%d,%d)\n", i, squares[i].get_row(),squares[i].get_col());
            tetrixmap->col_maj[squares[i].get_col()]->insert(squares[i].get_row());
        }
    }

    void reset(){
        ref_row = -2;
        ref_col = -1;
        ground_step = 0;
        for(int i=0;i<4;i++){
            squares[i] = square();
        }
    }
};


int main(int argc, char *argv[]){
    int rows, cols;
    int blkpos, blkgstep;
    char type[3];
    block blk;

    if(argc != 2) {
        printf("Error: amount of file in file out is wrong\n");
        exit(8);
    }
    ifstream infile(argv[1],ios::in);
    ofstream outfile("108000201_proj1.final",ios::out);

    infile >> rows >> cols;
    tetrixmap = new tetrix_map(rows+4,cols);
    while(1){
        infile >> type;
        if(type[0] == 'E') break;
        infile >> blkpos >> blkgstep;
        blk.reset();
        blk.set(type,blkpos-1,blkgstep);
        // printf("#type=%s, startpos=%d, crabstep=%d\n", type, blkpos, blkgstep);
        blk.drop();
        blk.crab(blk.ground_step);
        blk.drop();
        blk.stick_on_map();
        if(tetrixmap->check_bingo()){
            while(tetrixmap->four_bingo());
        }
        if(tetrixmap->is_death()){
            printf("Error: DEATH!\n");
            exit(9);
        }
        // tetrixmap->print_out();
        // cout << endl;
        // tetrixmap->detail_print();
        // printf("---------\n\n");
    }
    outfile << tetrixmap->print_out();
}