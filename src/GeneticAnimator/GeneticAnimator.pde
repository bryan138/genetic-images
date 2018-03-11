import java.util.Arrays;

static final String DNA_PATH = "../../../test/dna_quiroz.txt";
static final int SCALE = 2;

static final int CIRCLES_PER_STEP = 5;
static final int STEP = 10;
static final float SPEED = 0.1;

ArrayList<Circle> circles;
int count;
int active;
int fixed;

void setup() {
    size(512, 512);

    circles = new ArrayList<Circle>();
    byte[] data = loadBytes(DNA_PATH);

    count = 0;
    active = 0;
    fixed = -1;

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

    if (count % STEP == 0) {
        active += CIRCLES_PER_STEP;
    }

    boolean fix = true;
    for (int i = fixed + 1; i < active && i < circles.size(); i ++) {
        Circle circle = circles.get(i);
        circle.move();
        circle.display();

        fix = fix && circle.done;
        if (fix && i > fixed) {
            fixed = i;
            loadPixels();
        }
    }

    count ++;
}
