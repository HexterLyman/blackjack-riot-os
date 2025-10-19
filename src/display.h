

//#include "lcd.h"
#include "mineplex.h"

//hit box
#define XHIT 58
#define YHIT 102

//stand box
#define XSTAND 86
#define YSTAND 102

//cursor boundary
#define XBOUND 48
#define YBOUND 97

#define BOUND_LENGTH 67
#define BOUND_WIDTH 29


//cursor boundary (level = Bet)
#define BOUND_LENGTH_BET 94

#define XBOUND_BET 34

//+ box (level = Bet)
#define XADD 72
#define YADD 102

//- box (level = Bet)
#define XMIN 44
#define YMIN 102

//ok box (level = Bet)
#define XOK 100
#define YOK 102

//160
//128

enum Stage{
    PLAYER_TURN,
    DEALER_TURN,
    PLAYER_WIN,
    PLAYER_LOSE,
    PUSH,
    NATURAL_PLAYER,
    NATURAL_DEALER,
};

enum Level{
    START,
    BET,
    PLAY,
    HIGHSCORE,
    GAME_OVER
};

const uint16_t BACKGROUND = 0x0203; //casino table colour

void display_char(lcd_t dev, uint32_t xstart, uint32_t ystart, char ch, uint16_t f_color, uint16_t bg_color){
    const uint8_t * mineplex = mineplex_char(ch);

    uint16_t pic[25];

    for (int i = 0; i<5; i++){
        for (int j = 0 ; j<5; j++){
            int result = mineplex[i] & (1 << j); //ultimate weapon
            if (result > 0){
                pic[5*i+j] = f_color;
                } else {
                pic[5*i+j] = bg_color;
                }
            }
        }

    lcd_pixmap(&dev, xstart, xstart + 4, ystart, ystart + 4, pic);
}

void generate_box(lcd_t dev, uint32_t x_start, uint32_t y_start, uint16_t color){
    uint32_t l = 19; // length of lines (x-axis)
    uint32_t w = 19; // width of lines (y-axis)
    uint32_t t = 0;  //thickness of lines

    lcd_fill(&dev, x_start + l - t, x_start + l, y_start, y_start + w, color); //A
    lcd_fill(&dev, x_start, x_start + l, y_start, y_start + t, color); //B
    lcd_fill(&dev, x_start, x_start + t, y_start, y_start + w, color); //C
    lcd_fill(&dev, x_start, x_start + l,  y_start + w - t, y_start + w, color); //D
}

void generate_boundaries(lcd_t dev, uint32_t x_start, uint32_t y_start, uint16_t color){

    uint32_t l = BOUND_LENGTH; // length of lines (x-axis)
    uint32_t w = BOUND_WIDTH; // width of lines (y-axis)
    uint32_t t = 0;  //thickness of lines

    lcd_fill(&dev, x_start + l - t, x_start + l, y_start, y_start + w, color); //A
    lcd_fill(&dev, x_start, x_start + l, y_start, y_start + t, color); //B
    lcd_fill(&dev, x_start, x_start + t, y_start, y_start + w, color); //C
    lcd_fill(&dev, x_start, x_start + l,  y_start + w - t, y_start + w, color); //D
}

void generate_boundaries_bet(lcd_t dev, uint32_t x_start, uint32_t y_start, uint16_t color){

    uint32_t l = BOUND_LENGTH_BET; // length of lines (x-axis)
    uint32_t w = BOUND_WIDTH; // width of lines (y-axis)
    uint32_t t = 0;  //thickness of lines

    lcd_fill(&dev, x_start + l - t, x_start + l, y_start, y_start + w, color); //A
    lcd_fill(&dev, x_start, x_start + l, y_start, y_start + t, color); //B
    lcd_fill(&dev, x_start, x_start + t, y_start, y_start + w, color); //C
    lcd_fill(&dev, x_start, x_start + l,  y_start + w - t, y_start + w, color); //D
}


void generate_card(lcd_t dev, uint32_t x_start, uint32_t y_start, char ch){

    uint32_t l = 24; // length of lines (x-axis)
    uint32_t w = 36; // width of lines (y-axis)
    uint32_t t = 0;  //thickness of lines
    uint16_t color = 0x0000;
    uint32_t tens = 0;

    //black
    lcd_fill(&dev, x_start + l - t, x_start + l, y_start, y_start + w, color); //A
    lcd_fill(&dev, x_start, x_start + l, y_start, y_start + t, color); //B
    lcd_fill(&dev, x_start, x_start + t, y_start, y_start + w, color); //C
    lcd_fill(&dev, x_start, x_start + l,  y_start + w - t, y_start + w, color); //D

    //white
    lcd_fill(&dev, x_start+1, x_start + 23, y_start+1, y_start + 35, 0xffff); //white
    
    if(ch == 'X'){
        tens = random_uint32_range(0,4); //0-3 
        switch(tens){
        case 0:
            ch = 'X';
            break;
        case 1:
            ch = 'J';
            break;
        case 2:
            ch = 'Q';
            break;
        case 3:
            ch = 'K';
            break;
    }
    }
    if (ch == 'X') {
        display_char(dev, x_start + 3, y_start + 3, '1', 0x0000, 0xffff); //topleft
        display_char(dev, x_start + 8, y_start + 3, '0', 0x0000, 0xffff); //topleft

        display_char(dev, x_start + 11, y_start + 28, '1', 0x0000, 0xffff); //bottomright
        display_char(dev, x_start + 16, y_start + 28, '0', 0x0000, 0xffff); //bottomright
        return;
    }


    display_char(dev, x_start + 3, y_start + 3, ch, 0x0000, 0xffff); //topleft

    display_char(dev, x_start + 16, y_start + 28, ch, 0x0000, 0xffff); //bottomright

}

void generate_card_down(lcd_t dev, uint32_t x_start, uint32_t y_start){ //for dealer only

    uint32_t l = 24; // length of lines (x-axis)
    uint32_t w = 36; // width of lines (y-axis)
    uint32_t t = 0;  //thickness of lines
    uint16_t outline_color = 0x0000;

    //black
    lcd_fill(&dev, x_start + l - t, x_start + l, y_start, y_start + w, outline_color); //A
    lcd_fill(&dev, x_start, x_start + l, y_start, y_start + t, outline_color); //B
    lcd_fill(&dev, x_start, x_start + t, y_start, y_start + w, outline_color); //C
    lcd_fill(&dev, x_start, x_start + l,  y_start + w - t, y_start + w, outline_color); //D

    //pinkish-red
    lcd_fill(&dev, x_start+1, x_start + 23, y_start+1, y_start + 35, 0x0180); //pinkish-red
}

bool is_inside(uint32_t x_start, uint32_t y_start, uint32_t xc, uint32_t yc){
    uint32_t cr = 3; //cursor size
    uint32_t l = 18; //box size
    uint32_t w = 15; //box size

    return (((yc - cr) > y_start) && ((yc + cr) < (y_start + w)) && ((xc - cr) > x_start) && ((xc + cr) < (x_start + l)));

}


uint32_t limit_cursor(uint32_t x_start, uint32_t xc, uint32_t boundary_length){ //limits cursor to a specific boundary
    if(xc < (x_start + 3)){
        xc = x_start + 3;
    }
    else if(xc > (x_start + boundary_length - 3)){
        xc = x_start + boundary_length - 3;
    }

    return xc;
    // same with yc
}

void draw_cursor(lcd_t dev, lsm6dsxx_3d_data_t acc_value, enum Level level, uint16_t* xcpt, uint16_t* ycpt ) {
        
        uint16_t xc = *xcpt;
        uint16_t yc = *ycpt;
        uint16_t xc_prev = xc;
        uint16_t yc_prev = yc;
                    
        int max_acc = 500;
        if (acc_value.x < -1*max_acc) {
            //minimum value, prevent underflow
            xc = 3;
        } else if (acc_value.x > max_acc) {
            //maximum value, prevent overflow
            xc = dev.params->lines - 3;
        } else {
            xc = dev.params->lines * acc_value.x / (2*max_acc) + dev.params->lines / 2;
        }

        if (acc_value.y < -1*max_acc) {
            //minimum value, prevent underflow
            yc = 3;
        } else if (acc_value.y > max_acc) {
            //maximum value, prevent overflow
            yc = dev.params->rgb_channels - 3;
        } else {
            yc = dev.params->rgb_channels * acc_value.y / (2*max_acc) + dev.params->rgb_channels / 2;
        }

        // flip left-right-top-bottom
        // xc = 160 - xc
        xc = dev.params->lines - xc;
        // yc = 128 - yc                
        yc = dev.params->rgb_channels - yc;
        
        if (level == PLAY || level == GAME_OVER){
            xc = limit_cursor(XBOUND, xc, BOUND_LENGTH); //boundary length = 67
            yc = limit_cursor(YBOUND, yc, BOUND_WIDTH); //boundary widht = 29
        }

        //limit cursor at boundaries
        else if (level == BET){
            xc = limit_cursor(XBOUND_BET, xc, BOUND_LENGTH_BET); //boundary length = 67
            yc = limit_cursor(YBOUND, yc, BOUND_WIDTH); //boundary widht = 29
        }

        *xcpt = xc;
        *ycpt = yc;

        if (level == BET || level == PLAY || level == GAME_OVER
        ) {//draw cursor
            //"delete" (fill with background color) parts of the cursor
            if (xc > xc_prev){
                lcd_fill(&dev, xc_prev - 3, xc - 3, yc_prev - 3, yc_prev + 3, BACKGROUND);
            } else {
                lcd_fill(&dev, xc + 3, xc_prev + 3, yc_prev - 3, yc_prev + 3, BACKGROUND);
            }

            if (yc > yc_prev){
                lcd_fill(&dev, xc_prev - 3, xc_prev + 3, yc_prev - 3, yc - 3, BACKGROUND);
            } else {
                lcd_fill(&dev, xc_prev - 3, xc_prev + 3, yc + 3, yc_prev + 3, BACKGROUND);
            }

            lcd_fill(&dev, xc - 3, xc + 3, yc - 3, yc + 3, 0x07e0); //show cursor after flipping!
        }
}

void generate_player_value(lcd_t dev, uint8_t* score){
    uint32_t xstart = 120;
    uint32_t ystart = 67;

    display_char(dev, xstart, ystart, 'p', 0xffff, 0x0203);
    display_char(dev, xstart+5, ystart, 'l', 0xffff, 0x0203);
    display_char(dev, xstart+10, ystart, 'a', 0xffff, 0x0203); 
    display_char(dev, xstart+15, ystart, 'y', 0xffff, 0x0203);
    display_char(dev, xstart+20, ystart, 'e', 0xffff, 0x0203);           
    display_char(dev, xstart+25, ystart, 'r', 0xffff, 0x0203);
    display_char(dev, xstart+30, ystart, ':', 0xffff, 0x0203);

    //display score
    char second_digit = *score % 10 + 48; // +48 for converting to char
    char first_digit = *score / 10 + 48;
    display_char(dev, 134, 75, first_digit, 0xffff, 0x0203);
    display_char(dev, 139, 75, second_digit, 0xffff, 0x0203);
}

void generate_dealer_value(lcd_t dev, uint8_t* score){
    uint32_t xstart = 120;
    uint32_t ystart = 20;

    display_char(dev, xstart, ystart, 'd', 0xffff, 0x0203);
    display_char(dev, xstart+5, ystart, 'e', 0xffff, 0x0203);
    display_char(dev, xstart+10, ystart, 'a', 0xffff, 0x0203); 
    display_char(dev, xstart+15, ystart, 'l', 0xffff, 0x0203);
    display_char(dev, xstart+20, ystart, 'e', 0xffff, 0x0203);           
    display_char(dev, xstart+25, ystart, 'r', 0xffff, 0x0203);
    display_char(dev, xstart+30, ystart, ':', 0xffff, 0x0203);

    //display score
    char second_digit = *score % 10 + 48; // +48 for converting to char
    char first_digit = *score / 10 + 48;
    display_char(dev, 134, 28, first_digit, 0xffff, 0x0203);
    display_char(dev, 139, 28, second_digit, 0xffff, 0x0203);
}

void win_lose_display(lcd_t dev, enum Stage win_type){


    uint32_t xstart = 0;
    uint32_t ystart = 0;

    switch(win_type){

        case PLAYER_WIN: //win
            xstart = 11;
            ystart = 110;

            display_char(dev, xstart, ystart, 'W', 0xffff, 0x0203);
            display_char(dev, xstart+5, ystart, 'I', 0xffff, 0x0203);
            display_char(dev, xstart+10, ystart, 'N', 0xffff, 0x0203); 
            display_char(dev, xstart+15, ystart, '!', 0xffff, 0x0203);
            break;

        case NATURAL_PLAYER: //natural 21 player
            xstart = 15;
            ystart = 110;

            display_char(dev, xstart, ystart, '2', 0xffff, 0x0203);
            display_char(dev, xstart+5, ystart, '1', 0xffff, 0x0203);
            display_char(dev, xstart+10, ystart, '!', 0xffff, 0x0203); 
            break;

        case PLAYER_LOSE: //lose
            xstart = 8;
            ystart = 110;

            display_char(dev, xstart, ystart, 'L', 0xffff, 0x0203);
            display_char(dev, xstart+5, ystart, 'O', 0xffff, 0x0203);
            display_char(dev, xstart+10, ystart, 'S', 0xffff, 0x0203); 
            display_char(dev, xstart+15, ystart, 'E', 0xffff, 0x0203);
            display_char(dev, xstart+20, ystart, '!', 0xffff, 0x0203);
            break;    
            
        case NATURAL_DEALER: //lose
            xstart = 8;
            ystart = 110;

            display_char(dev, xstart, ystart, 'L', 0xffff, 0x0203);
            display_char(dev, xstart+5, ystart, 'O', 0xffff, 0x0203);
            display_char(dev, xstart+10, ystart, 'S', 0xffff, 0x0203); 
            display_char(dev, xstart+15, ystart, 'E', 0xffff, 0x0203);
            display_char(dev, xstart+20, ystart, '!', 0xffff, 0x0203);
            break;   
            
        case PUSH: //push
            xstart = 8;
            ystart = 110;

            display_char(dev, xstart, ystart, 'P', 0xffff, 0x0203);
            display_char(dev, xstart+5, ystart, 'U', 0xffff, 0x0203);
            display_char(dev, xstart+10, ystart, 'S', 0xffff, 0x0203); 
            display_char(dev, xstart+15, ystart, 'H', 0xffff, 0x0203);
            display_char(dev, xstart+20, ystart, '!', 0xffff, 0x0203);
            break;

        default: //for other modes
            break;
    }

}

void score_adder(uint32_t current_card_value, uint8_t* total_card_score, bool* has_ace){

    if(current_card_value == 0){
        *has_ace = true;
        *total_card_score += 11;
    }

    else{
    *total_card_score += (current_card_value+1);
    }

    if(*total_card_score > 21 && *has_ace){
        *total_card_score -= 10;
        *has_ace = false;
    }

}

//will delete
void bet_display(lcd_t dev, uint32_t bet_value){

    uint32_t x_bet = 129;
    uint32_t y_bet = 104;

    uint32_t x_val = 130;
    uint32_t y_val = 112;

    //display bet sign

    display_char(dev, x_bet, y_bet, 'B', 0xffff, 0x0203);
    display_char(dev, x_bet+5, y_bet, 'E', 0xffff, 0x0203);
    display_char(dev, x_bet+10, y_bet, 'T', 0xffff, 0x0203); 
    display_char(dev, x_bet+15, y_bet, ':', 0xffff, 0x0203);

    //display value
    char third_digit = bet_value % 10 + 48; // +48 for converting to char
    bet_value /= 10;
    char second_digit = bet_value % 10 + 48;
    bet_value /= 10;
    char first_digit = bet_value % 10 + 48;
    
    display_char(dev, x_val, y_val, first_digit, 0xffff, 0x0203);
    display_char(dev, x_val+5, y_val, second_digit, 0xffff, 0x0203);
    display_char(dev, x_val+10, y_val, third_digit, 0xffff, 0x0203);            
}

void bet_display_text(lcd_t dev, uint32_t x_bet, uint32_t y_bet) {
    display_char(dev, x_bet, y_bet, 'B', 0xffff, 0x0203);
    display_char(dev, x_bet+5, y_bet, 'E', 0xffff, 0x0203);
    display_char(dev, x_bet+10, y_bet, 'T', 0xffff, 0x0203); 
    display_char(dev, x_bet+15, y_bet, ':', 0xffff, 0x0203);
}

void bet_display_value(lcd_t dev, uint32_t bet_value, uint32_t x_val, uint32_t y_val) {
    //display value
    char third_digit = bet_value % 10 + 48; // +48 for converting to char
    bet_value /= 10;
    char second_digit = bet_value % 10 + 48;
    bet_value /= 10;
    char first_digit = bet_value % 10 + 48;
    
    display_char(dev, x_val, y_val, first_digit, 0xffff, 0x0203);
    display_char(dev, x_val+5, y_val, second_digit, 0xffff, 0x0203);
    display_char(dev, x_val+10, y_val, third_digit, 0xffff, 0x0203);    
}



void display_cash_value(lcd_t dev, uint32_t cash_value, uint32_t x, uint32_t y){

    uint32_t x_text = x;
    uint32_t y_text = y;

    uint32_t x_value = x + 33;
    uint32_t y_value = y;
    
    char fourth_digit = cash_value % 10 + 48;
    cash_value /= 10;
    char third_digit = cash_value % 10 + 48; // +48 for converting to char
    cash_value /= 10;
    char second_digit = cash_value % 10 + 48;
    cash_value /= 10;
    char first_digit = cash_value % 10 + 48;


    //cash:
    display_char(dev, x_text, y_text, 'C', 0xffff, 0x0203);
    display_char(dev, x_text+5, y_text, 'A', 0xffff, 0x0203);
    display_char(dev, x_text+10, y_text, 'S', 0xffff, 0x0203); 
    display_char(dev, x_text+15, y_text, 'H', 0xffff, 0x0203);
    display_char(dev, x_text+20, y_text, ':', 0xffff, 0x0203);



    //value display
    display_char(dev, x_value, y_value, first_digit, 0xffff, 0x0203);
    display_char(dev, x_value+5, y_value, second_digit, 0xffff, 0x0203);
    display_char(dev, x_value+10, y_value, third_digit, 0xffff, 0x0203); 
    display_char(dev, x_value+15, y_value, fourth_digit, 0xffff, 0x0203);

}


void display_min_max(lcd_t dev, uint32_t x_min, uint32_t y_min, uint32_t x_max, uint32_t y_max){

    uint32_t x_min_text = x_min;
    uint32_t y_min_text = y_min;

    uint32_t x_min_10 = x_min_text + 26;
    uint32_t y_min_10 = y_min_text;

    uint32_t x_max_text = x_max;
    uint32_t y_max_text = y_max;

    uint32_t x_max_100 = x_max_text + 26;
    uint32_t y_max_100 = y_max_text;

    //MIN:
    display_char(dev, x_min_text, y_min_text, 'M', 0xffff, 0x0203);
    display_char(dev, x_min_text+5, y_min_text, 'I', 0xffff, 0x0203);
    display_char(dev, x_min_text+10, y_min_text, 'N', 0xffff, 0x0203); 
    display_char(dev, x_min_text+15, y_min_text, ':', 0xffff, 0x0203);

    //min value 10
    display_char(dev, x_min_10, y_min_10, '1', 0xffff, 0x0203);
    display_char(dev, x_min_10+5, y_min_10, '0', 0xffff, 0x0203);

    //MAX:
    display_char(dev, x_max_text, y_max_text, 'M', 0xffff, 0x0203);
    display_char(dev, x_max_text+5, y_max_text, 'A', 0xffff, 0x0203);
    display_char(dev, x_max_text+10, y_max_text, 'X', 0xffff, 0x0203); 
    display_char(dev, x_max_text+15, y_max_text, ':', 0xffff, 0x0203);

    //max value 100
    display_char(dev, x_max_100, y_max_100, '1', 0xffff, 0x0203);
    display_char(dev, x_max_100+5, y_max_100, '0', 0xffff, 0x0203);
    display_char(dev, x_max_100+10, y_max_100, '0', 0xffff, 0x0203);


}

void display_bet_options(lcd_t dev, uint32_t x_min, uint32_t y_min, uint32_t x_add, uint32_t y_add, uint32_t x_ok, uint32_t y_ok){
    
    //minus bet
    display_char(dev, x_min, y_min, '-', 0xffff, 0x0203);

    //add bet
    display_char(dev, x_add, y_add, '+', 0xffff, 0x0203);

    //ok bet
    display_char(dev, x_ok, y_ok, 'O', 0xffff, 0x0203);
    display_char(dev, x_ok+5, y_ok, 'K', 0xffff, 0x0203);
}


//HIGHSCORE
void display_yes_no(lcd_t dev){

    uint32_t x_play = 70;
    uint32_t y_play = 44;

    uint32_t x_again = 64;
    uint32_t y_again = 52;

    uint32_t x_yes = 59;
    uint32_t y_yes = 83;

    uint32_t x_no = 90;
    uint32_t y_no = 83;


    display_char(dev, x_play, y_play, 'P', 0xffff, 0x0203);
    display_char(dev, x_play+5, y_play, 'L', 0xffff, 0x0203);
    display_char(dev, x_play+10, y_play, 'A', 0xffff, 0x0203); 
    display_char(dev, x_play+15, y_play, 'Y', 0xffff, 0x0203);

    display_char(dev, x_again, y_again, 'A', 0xffff, 0x0203);
    display_char(dev, x_again+5, y_again, 'G', 0xffff, 0x0203);
    display_char(dev, x_again+10, y_again, 'A', 0xffff, 0x0203); 
    display_char(dev, x_again+15, y_again, 'I', 0xffff, 0x0203);
    display_char(dev, x_again+20, y_again, 'N', 0xffff, 0x0203);
    display_char(dev, x_again+25, y_again, '?', 0xffff, 0x0203);
    
    display_char(dev, x_yes, y_yes, 'Y', 0xffff, 0x0203);
    display_char(dev, x_yes+5, y_yes, 'E', 0xffff, 0x0203);
    display_char(dev, x_yes+10, y_yes, 'S', 0xffff, 0x0203); 

    display_char(dev, x_no, y_no, 'N', 0xffff, 0x0203);
    display_char(dev, x_no+5, y_no, 'O', 0xffff, 0x0203);
    

}

void display_score(lcd_t dev, uint32_t xstart, uint32_t ystart, uint32_t number){
    for(int i = 3; i >= 0; i--) {
        int digit = number % 10; // take the last digit first 

        char c = digit + 48; //convert to char

        display_char(dev, xstart + 6 * i , ystart, c, 0xffff, 0x0203); // 6*i to position the digit at last position

        number /= 10; //eliminate the last digit
    }
}

void display_highscore(lcd_t dev, uint32_t highscore[]) {
    uint32_t highscore_xstart = 49;
    uint32_t highscore_ystart = 26;
    display_char(dev, highscore_xstart, highscore_ystart, 'H', 0xffff, 0x0203);
    display_char(dev, highscore_xstart+6, highscore_ystart, 'I', 0xffff, 0x0203);
    display_char(dev, highscore_xstart+12, highscore_ystart, 'G', 0xffff, 0x0203);
    display_char(dev, highscore_xstart+18, highscore_ystart, 'H', 0xffff, 0x0203);

    display_char(dev, highscore_xstart+30, highscore_ystart, 'S', 0xffff, 0x0203);
    display_char(dev, highscore_xstart+36, highscore_ystart, 'C', 0xffff, 0x0203);
    display_char(dev, highscore_xstart+42, highscore_ystart, 'O', 0xffff, 0x0203);
    display_char(dev, highscore_xstart+48, highscore_ystart, 'R', 0xffff, 0x0203);
    display_char(dev, highscore_xstart+54, highscore_ystart, 'E', 0xffff, 0x0203);
    display_char(dev, highscore_xstart+60, highscore_ystart, 'S', 0xffff, 0x0203);
    
    for (int i = 0; i<5;i++){
        display_char(dev, highscore_xstart, highscore_ystart + 10*(i+1), i+49, 0xffff, 0x0203);
        display_char(dev, highscore_xstart+5, highscore_ystart + 10*(i+1), '.', 0xffff, 0x0203);
        display_score(dev, highscore_xstart+15, highscore_ystart + 10*(i+1), highscore[i]);
    }
}