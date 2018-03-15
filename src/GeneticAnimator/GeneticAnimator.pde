import java.util.Arrays;

static final String DNA_PATH = "../../../test/dna_monalisa_2018.03.12_09.57.11.gi";
static final int SCALE = 2;

static final int CIRCLES_PER_STEP = 3;
static final int STEP = 4;
static final float SPEED = 0.1;

static final boolean DEBUG = true;

ArrayList<Circle> circles;
int count;
int active;
int done;

void setup() {
    size(512, 512);

    circles = new ArrayList<Circle>();
    byte[] data = loadBytes(DNA_PATH);

    count = 0;
    active = 0;
    done = -1;

    noStroke();
    for (int i = 0; i + 7 <= data.length; i += 7) {
        float alpha = data[i + 3] & 0xFF;
        if (alpha > 0) {
            Circle circle = new Circle(Arrays.copyOfRange(data, i, i + 7));
            circles.add(circle);
        }
    }
    println("TOTAL CIRCLES: " + circles.size());
}

void draw() {
    background(0);
    updatePixels();

    int step;
    float progress = done * 1.0 / circles.size();
    if (progress < 0.05) {
       step = (int)(STEP * 2.0);
    } else if (progress < 0.1) {
       step = (int)(STEP * 1.5);
    } else if (progress < 0.25) {
        step = (int)(STEP * 1.0);
    } else if (progress < 0.5) {
        step = (int)(STEP * 0.85);
    } else if (progress < 0.75) {
        step = (int)(STEP * 0.65);
    } else {
        step = (int)(STEP * 0.5);
    }

    count ++;
    if (count % step == 0) {
        active += CIRCLES_PER_STEP;
    }

    boolean fix = true;
    for (int i = done + 1; i < active && i < circles.size(); i ++) {
        Circle circle = circles.get(i);
        circle.move();
        circle.display();

        fix = fix && circle.done;
        if (fix && i > done) {
            done = i;
            loadPixels();
        }
    }

    if (DEBUG) {
        fill(255);
        text(step + " " + progress, 10, 20);
    }
}
