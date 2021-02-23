#ifndef SOME_FUN_H
#define SOME_FUN_H

int detectEdge(int pin_value, bool up_down = true)
{
    static int past_value = 0;

    if(pin_value && !past_value)
    {
        past_value = 1;
        delayMicroseconds(50);

        if(up_down)
            return 1;
    }
    else if(!pin_value && past_value)
    {
        past_value = 0;
        delayMicroseconds(50);

        if(!up_down)
            return 1;
    }

    return 0;
}

void changeLED(int button_pin, int led_pin)
{
    int proslo;
    int edge, pin_status;

    // pin
    proslo = digitalRead(led_pin);

    // detect edge
    pin_status = digitalRead(button_pin);
    edge = detectEdge(pin_status, false);

    // hold led
    if(edge && !proslo)
    {
        digitalWrite(led_pin, HIGH);
        proslo = 1;
    }
    else if(edge && proslo)
    {
        digitalWrite(led_pin, LOW);
        proslo = 0;
    }
}

#endif // SOME_FUN_H