/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *               2021 Francisco Molina
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for lcd tft displays
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

 #include <stdio.h>
 #include <stdint.h> //to use int16_t and uint16_t
 #include <fcntl.h> //file control
 #include "timex.h"
 #include "ztimer.h"
 #include "benchmark.h"
 #include "board.h"
 #include "lcd.h"
 #include "lcd_internal.h"
 #include "busy_wait.h"
 #include "lsm6dsxx.h"
 #include "lsm6dsxx_params.h"
 #include "random.h"
// #include "mineplex.h"
 #include "string.h"
 #include "core/display.h"
 #include "core/start_menu.h"
 #include "core/casino_enter_animation.h"
 #include "fs/constfs.h"
 
//  #include "riot_logo.h"
 // #include "lvgl.h"
 
 #include "st77xx.h"
 #include "st77xx_params.h"

 #define BTN_PIN GPIO_PIN(0, 2)
 #define BTN_MODE GPIO_IN_PU
 // #include "seminar_riot_os.h"

 
bool RESET_HIGHSCORE_ON_RESET = false;

static vfs_mount_t const_mount = {
    .fs = &constfs_file_system,
    .mount_point = "/const"
};

/*

//initialize box "hit" starting point
uint32_t xh_start = 58; //move to display.h as #define 
uint32_t yh_start = 102; //move to display.h as #define 

//initialize box "stand" starting point
uint32_t xs_start = 86; //move to display.h as #define 
uint32_t ys_start = 102; //move to display.h as #define 

//initialize boundary box for cursor
uint32_t xb_start = 48; //move to display.h as #define 
uint32_t yb_start = 97; //move to display.h as #define 

*/
//TODO: try display 10 since it is not char! DONE
char card_number[10] = {'A','2','3','4','5','6','7','8','9','X'};

uint8_t player_score = 0; 
uint8_t dealer_score = 0; 

bool init = true;

enum Stage myStage = PLAYER_TURN; //change later when adding new scenes
enum Level level = START;


bool has_ace_player = false;
bool has_ace_dealer = false;
bool suspicious_ace_dealer = false;


int player_card = 0;
int dealer_card = 0;

uint32_t bet = 10;
uint32_t cash = 1000;

char highscore_str[20]; 


int main(void) {
    // ==================================================
    // MOUNTING
    // printf("mounting....\r\n");
    
    int res = vfs_mount(&const_mount);
    if (res < 0) {
        printf("Error while mounting constfs\r\n");
    }
    else {
        printf("constfs mounted successfully\r\n");
    }

    FILE *file;

    // ==================================================
    // WRITE
    if (RESET_HIGHSCORE_ON_RESET) {
        char highscoretest[20] = "00000000000000000000";
        size_t n = sizeof(highscoretest) / sizeof(highscoretest[0]);
        file = fopen("/nvm0/highscore.txt", "w");
        if (file == NULL) {
            printf("Error opening file");
            return 1;
        }
        fwrite(highscoretest, sizeof(char), n, file);    
        fclose(file);
    }

    // ===================================================
    // READ
    file = fopen("/nvm0/highscore.txt", "r");
    if (file == NULL) {
        printf("Error opening file");
        return 1;
    }
    size_t result = fread(highscore_str, sizeof(char), 20, file);
    if (result != 20){
        printf("Error reading file");
        return 1;
    }
    fclose(file);
    // ===================================================

    //printf("cat highscore done....\r\n");
    //printf("highscore_txt: %s\r\n", highscore_str);

    // load highscore array from highscore_str
    uint32_t highscore[5]; //highscore list (not digits)

    for (int i=0;i<5;i++){
        int num = 0;
        for (int j=0; j<4; j++){
            char score_char = highscore_str[4*i + j];
            int score_digit = score_char - 48; //convert char to int
            num *= 10;
            num += score_digit;
        }
        highscore[i] = num;
    }


     lcd_t dev; //to initialize our device
     lsm6dsxx_t dev2;
     dev.driver = &lcd_st77xx_driver;
     lsm6dsxx_3d_data_t acc_value;

    gpio_init(BTN_PIN, BTN_MODE);

    //flasher DO NOT REMOVE!
    if (lsm6dsxx_init(&dev2, lsm6dsxx_params) != LSM6DSXX_OK) {
        puts("[ERROR]");
        return 1;
    }
    puts("[SUCCESS]\n");

    puts("Powering down LSM6DSXX sensor...");
    if (lsm6dsxx_acc_power_down(&dev2) != LSM6DSXX_OK) {
        puts("[ERROR]");
        return 1;
    }
    puts("[SUCCESS]\n");

    ztimer_sleep(ZTIMER_MSEC, 1 * 1000);

    puts("Powering up LSM6DSXX sensor...");
    if (lsm6dsxx_acc_power_up(&dev2) != LSM6DSXX_OK) {
        puts("[ERROR]");
        return 1;
    }
    puts("[SUCCESS]\n");


     puts("lcd TFT display test application");
 
     /* initialize the sensor */
     printf("Initializing display...");
 
     /* Enable backlight if macro is defined */
     /*
    #ifdef BACKLIGHT_ON
     BACKLIGHT_ON;
    #endif
    */
     if (lcd_init(&dev, &st77xx_params[0]) == 0) {
         puts("[OK]");
     }
     else {
         puts("[Failed]");
         return 1;
     }
    
    puts("lcd TFT display filling map");
    lcd_fill(&dev, 0, dev.params->lines, 0, dev.params->rgb_channels, BACKGROUND);
    puts("lcd TFT display map filled");
    //end of flasher

    //initialize cursor origin
    
    uint16_t xc = dev.params->lines/2;
    uint16_t yc = dev.params->rgb_channels/2;

    

    // uint32_t card_x = 14;
    // uint32_t card_y = 83;

     
    int btn_pin_prev = 1; //1 represents unpressed state
    
    
    uint32_t d0 = 0;
    uint32_t d1 = 0; 

      while (1) {
        //printf("Level = %d\r\n", level);
        if (lsm6dsxx_read_acc(&dev2, &acc_value) == LSM6DSXX_OK) {

        //draw cursor
        draw_cursor(dev, acc_value, level, &xc, &yc);

        if (level == START && init) {
            lcd_fill(&dev, 0, dev.params->lines, 0, dev.params->rgb_channels, 0x0000);
            lcd_pixmap(&dev, 0, 159, 0, 127, menu_bitmap);
            init = false;
        }

        if (level == START) {
            if ((gpio_read(BTN_PIN)) == 0 && (btn_pin_prev == 1)) { //press button to go to level = bet
                animate_casino_enter(dev);
                cash = 1000; //reset cash
                level = BET;
                init = true;
            }    
        }

        if (level == BET && init) {
            //reset bet
            bet = 10;
            lcd_fill(&dev, 0, dev.params->lines, 0, dev.params->rgb_channels, BACKGROUND);
            bet_display_text(dev, 59, 60);
            bet_display_value(dev, bet, 86, 60);
            display_cash_value(dev, cash, 54, 39);
            display_min_max(dev, 11,13,114,13);
            display_bet_options(dev, 51,88,79,88,104,88);
            init = false;
        }

        if (level == BET){
            generate_boundaries_bet(dev, XBOUND_BET, YBOUND, 0xffff); //border box
            generate_box(dev, XADD, YADD, 0x07e6); //add button
            generate_box(dev, XMIN, YMIN, 0x07e6); //min button
            generate_box(dev, XOK, YOK, 0x07e6); //ok button

            //press add button
            if ((gpio_read(BTN_PIN)) == 0 && (btn_pin_prev == 1) && is_inside(XADD, YADD, xc, yc) && bet <= 90 && bet <= cash - 10) { 
                bet += 10;
                bet_display_value(dev, bet, 86, 60);
            }

            //press minus button
            if ((gpio_read(BTN_PIN)) == 0 && (btn_pin_prev == 1) && is_inside(XMIN, YMIN, xc, yc) && bet > 10) {
                bet -= 10;
                bet_display_value(dev, bet, 86, 60);
            }

            //press ok button
            if ((gpio_read(BTN_PIN)) == 0 && (btn_pin_prev == 1) && is_inside(XOK, YOK, xc, yc) ) {
                level = PLAY;
                init = true;
            }            
        }


        if ( ((level == PLAY) || (level == GAME_OVER))  && (!init)){
            generate_boundaries(dev, XBOUND, YBOUND, 0xffff); //border box
            generate_box(dev, XHIT, YHIT, 0x07e6); //hit button
            generate_box(dev, XSTAND, YSTAND, 0x07e6); //stand button
        }

        if (level == PLAY && init) {
            //resetting values, ensure level=play starts with fresh values;
            myStage = PLAYER_TURN; //change later when adding new scenes

            dealer_score = 0;
            player_score = 0;

            has_ace_player = false;
            has_ace_dealer = false;
            suspicious_ace_dealer = false;

            player_card = 0;
            dealer_card = 0;

            //generate dealer card values
            lcd_fill(&dev, 0, dev.params->lines, 0, dev.params->rgb_channels, BACKGROUND);

            d0 = random_uint32_range(0,10); // 0 - 9
            d1 = random_uint32_range(0,10); // 0 - 9

            //generate player card values

            uint32_t p[2] = {0,9};

            for(int i = 0; i <= 1; i++){

                p[i] = random_uint32_range(0,10); // 0 - 9

                score_adder(p[i], &player_score, &has_ace_player);

            }
            
            score_adder(d0, &dealer_score, &has_ace_dealer);

            if(has_ace_dealer == true){
                suspicious_ace_dealer = true;
            }
            
            // cards are dealt
            generate_card(dev, 3, 55, card_number[p[0]]); //player 1st
            busy_wait_us(500000); //delay
            generate_card(dev, 3, 9, card_number[d0]); //dealer 1st
            busy_wait_us(500000); //delay
            generate_card(dev, 14, 55, card_number[p[1]]); //player 2nd
            busy_wait_us(500000); //delay
            generate_card_down(dev, 14, 9);// dealer 2nd (hidden)
            busy_wait_us(500000); //delay

            //natural 21 comparison

            if(suspicious_ace_dealer && (d1==9) && (player_score == 21)){ //dealer and player 21
                myStage = PUSH;
                generate_card(dev, 14, 9, card_number[d1]); //show dealer card
                score_adder(d1, &dealer_score, &has_ace_dealer);
                suspicious_ace_dealer = false;
            }
            else if(suspicious_ace_dealer && (d1==9) && (player_score != 21)){ //dealer 21, dealer shows ace
                myStage = NATURAL_DEALER;
                generate_card(dev, 14, 9, card_number[d1]); //show dealer card
                score_adder(d1, &dealer_score, &has_ace_dealer);
                suspicious_ace_dealer = false;
            }
            else if(suspicious_ace_dealer && (d1 != 9) && (player_score == 21)){ //player 21, dealer shows ace
                myStage = NATURAL_PLAYER;
                suspicious_ace_dealer = false;
            }
            else if(suspicious_ace_dealer && (d1 != 9) && (player_score != 21)){ //game continue as normal
                myStage = PLAYER_TURN;
                suspicious_ace_dealer = false;
            }
            else if(!suspicious_ace_dealer && (player_score == 21)){ //player 21, dealer no ace
                myStage = NATURAL_PLAYER;
                suspicious_ace_dealer = false;
            }
            

            player_card = 2;
            dealer_card = 1;

            //will delete
            //bet_display(dev, 100); //to display bet

            bet_display_text(dev, 129, 104); //text xbet = 129, ybet = 104
            bet_display_value(dev, bet, 130, 112); //value xval = 130, yval = 112
            init = false ;
        }

        if (level == PLAY) {
            generate_player_value(dev, &player_score); //display score first
            

            //input system for cursor: STATUS COMPLETE
            //player hits
            if ((gpio_read(BTN_PIN)) == 0 && (btn_pin_prev == 1) && is_inside(XHIT,YHIT,xc,yc) && (myStage == PLAYER_TURN)) {
                //printf("HIT!\n");
                if (player_card < 9){
                    uint32_t pn = random_uint32_range(0,10); // 0 - 9
                    generate_card(dev, 3 + 11*player_card, 55, card_number[pn]);

                    score_adder(pn, &player_score, &has_ace_player);
                
                    player_card++;

                    if(player_score > 21){
                        myStage = PLAYER_LOSE;
                    }
                }
            }
            
            //player_stands
            else if((gpio_read(BTN_PIN)) == 0 && (btn_pin_prev == 1) && is_inside(XSTAND,YSTAND,xc,yc) && (myStage == PLAYER_TURN)){
                myStage = DEALER_TURN;
                generate_card(dev, 3 + 11*dealer_card, 9, card_number[d1]);
                score_adder(d1, &dealer_score, &has_ace_dealer);
                generate_dealer_value(dev, &dealer_score);
                busy_wait_us(500000); //delay
                dealer_card++;
                
            }

            //dealer_automatic
            else if((myStage == DEALER_TURN)){
                //printf("STAND!\n");


                if (dealer_card < 9 && (dealer_score <= 16)){ //dealer hits when 16 and below

                    uint32_t pd = random_uint32_range(0,10); // 0 - 9
                    generate_card(dev, 3 + 11*dealer_card, 9, card_number[pd]);
                    score_adder(pd, &dealer_score, &has_ace_dealer);
                    dealer_card++;

                    if(dealer_score > 21){
                        myStage = PLAYER_WIN;
                    }
                    
                }

                else if(dealer_score >= 17){ //dealer stands and compares 

                    if(player_score > dealer_score){
                        myStage = PLAYER_WIN;
                    }
                    else if(player_score < dealer_score){
                        myStage = PLAYER_LOSE;
                    }
                    else{
                        myStage = PUSH;
                    }
                }

                generate_dealer_value(dev, &dealer_score);

            }




            generate_player_value(dev, &player_score);
            generate_dealer_value(dev, &dealer_score);

            if (myStage == DEALER_TURN) {
                busy_wait_us(500000); //delay
            }

            if((myStage == PLAYER_WIN) || (myStage == PLAYER_LOSE) || (myStage == NATURAL_PLAYER) || (myStage == NATURAL_DEALER) || (myStage == PUSH)){
                if (myStage == PLAYER_WIN) {
                    cash += bet;
                }

                else if (myStage == NATURAL_PLAYER) {
                    cash += 1.5 * bet;
                }

                else if (myStage == PLAYER_LOSE || myStage == NATURAL_DEALER) {
                    cash -= bet;
                }

                //if push do nothing

                win_lose_display(dev, myStage);
                for (int i = 0; i<2 ; i++){
                    busy_wait_us(500000); //delay
                }
                if (cash < 10) {
                    level = HIGHSCORE;
                } else {
                    level = GAME_OVER;
                }
                init = true;
            }
        }

        if (level == GAME_OVER){
            if (init) {
                lcd_fill(&dev, 0, dev.params->lines, 0, dev.params->rgb_channels, BACKGROUND);
                display_yes_no(dev);
                display_cash_value(dev, cash, 54, 18);
                init = false;
            }
            //player hit yes
            if ((gpio_read(BTN_PIN)) == 0 && (btn_pin_prev == 1) && is_inside(XHIT,YHIT,xc,yc)) {
                // to bet
                //printf("play again: ");
                level = BET;
                init = true;
                //printf("yes\r\n");
            }

            //player hit no
            else if((gpio_read(BTN_PIN)) == 0 && (btn_pin_prev == 1) && is_inside(XSTAND,YSTAND,xc,yc)) {
                //to highscore
                level = HIGHSCORE;
                init = true;
            }
        }

        if (level == HIGHSCORE && init) {
            bool should_write = false;
            lcd_fill(&dev, 0, dev.params->lines, 0, dev.params->rgb_channels, BACKGROUND);
            // compare current score (cash) to highscore[5]
            printf("GAME OVER -- cash: %ld\r\n", cash);
            
           
            for (int i = 4; i >=0; i--){
                if(cash < highscore[i]){
                    break;
                }

                if(i != 4){ //if i = 5 will error since not exist
                    highscore[i+1] = highscore[i];
                }
                highscore[i] = cash;
                should_write = true;
            }
            
            //WRITE
            if (should_write) {
                for (int i = 0; i < 5; i++){
                    int number_to_write = highscore[i];
                    for(int j = 3; j >= 0; j--){
                        int digit = number_to_write % 10;
                        char c = digit + 48;
                        highscore_str[i*4 + j] = c;
                        number_to_write /= 10;
                    }
                }

                //filesystem
                size_t n_out = sizeof(highscore_str) / sizeof(highscore_str[0]);
                file = fopen("/nvm0/highscore.txt", "w");
                if (file == NULL) {
                    printf("Error opening file");
                    return 1;
                }
                fwrite(highscore_str, sizeof(char), n_out, file);    
                fclose(file);
            }
            
            display_highscore(dev, highscore);
            init = false;
        }

        if ( level == HIGHSCORE ) {
            if((gpio_read(BTN_PIN)) == 0 && (btn_pin_prev == 1)) {
                //to start
                level = START;
                init = true;
            }
        }
        
        btn_pin_prev = gpio_read(BTN_PIN); //previously assigned inside if (level == PLAY) (line 425)
        busy_wait_us(1000);
    }
    } 
    return 0;
 }
 