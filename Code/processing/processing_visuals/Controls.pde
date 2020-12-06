// ------------------------ KEYS ---------------------------
void keyPressed()
{
        println(key + " pressed");
        switch (key) {
        case 'A':
                snare.wasHit = true;
                snare.record_String("※", (height-20-600)/4);
                break;

        case 'S':
                hihat.record_String("※", (height-20-600)/4);
                hihat.wasHit = true;
                println("s pressed.");

        case 'D':
                kick.record_String("※", (height-20-600)/4);
                kick.wasHit = true;
                break;
        case 'Q':
                crash1.record_String("※", (height-20-600)/4);
                crash1.wasHit = true;
                break;

        case 'W':
                ride.record_String("※", (height-20-600)/4);
                ride.wasHit = true;
                break;
        case 'E':
                // cowbell.record_String("※", (height-20-600)/4);
                // cowbell.wasHit = true;
                break;

        case 'Y':
                tom2.record_String("※", (height-20-600)/4);
                tom2.wasHit = true;
                break;
        case 'X':
                standtom1.record_String("※", (height-20-600)/4);
                standtom1.wasHit = true;
                break;
        }
}
