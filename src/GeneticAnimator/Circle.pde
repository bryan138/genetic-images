import java.util.Random;

static Random random = new Random();


class Circle {
    PVector location;
    float radius;
    float alpha;
    int r;
    int g;
    int b;

    PVector _location;
    float _radius;
    float _alpha;
    int _r;
    int _g;
    int _b;

    boolean done;

    Circle(byte[] data) {
        done = false;

        int dna[] = new int[7];
        for (int i = 0; i < dna.length; i ++) {
            dna[i] = data[i] & 0xFF;
        }

        location = new PVector(dna[0] * SCALE, dna[1] * SCALE);
        radius = dna[2] * SCALE;
        alpha = dna[3];
        r = dna[4];
        g = dna[5];
        b = dna[6];

        _location = new PVector(random.nextInt(256 * SCALE), random.nextInt(256 * SCALE));
        _radius = 0;
        _alpha = random.nextInt(256);
        _r = random.nextInt(256);
        _g = random.nextInt(256);
        _b = random.nextInt(256);
    }

    void display() {
        //*
        fill(color(_r, _g, _b, _alpha));
        ellipse(_location.x, _location.y, _radius * 2, _radius * 2);
        //*/

        /*
        fill(color(r, g, b, alpha));
        ellipse(location.x, location.y, radius * 2, radius * 2);
        done = true;
        //*/
    }

    void move() {
        if (!done) {
            PVector direction = PVector.sub(location, _location);
            if (direction.mag() > 1) {
                direction.mult(0.05);
                _location.add(direction);
            } else {
                _location = location;
            }

            _radius = ease(_radius, radius);
            _alpha = ease(_alpha, alpha);

            if (_r != r) _r = _r + ((r > _r) ? 1 : -1);
            if (_g != g) _g = _g + ((g > _g) ? 1 : -1);
            if (_b != b) _b = _b + ((b > _b) ? 1 : -1);

            done = (_location.x == location.x && _location.y == location.y && _radius == radius && _alpha == alpha && _r == r && _g == g && _b == b);
        }
    }

    float ease(float value, float target) {
        value += ((target - value) * SPEED);
        if (abs(target - value) < 1) {
            value =  target;
        }
        return value;
    }
}
