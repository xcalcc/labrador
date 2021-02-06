/*
 * GJB5369: 4.8.2.6
 * avoid using free statements
 */

void static_p(void){
    unsigned int Timing_Loop = 100u;
    Timing_Loop--;  /* free statement */
}