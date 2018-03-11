import java.util.Arrays;

static final String TEST_FOLDER = "../../../test/";
static final int SCALE = 2;

static final int CIRCLES_PER_STEP = 5;
static final int STEP = 10;

ArrayList<Circle> circles;
int index;
int count;
int active;
int fixed;

void setup() {
  size(512, 512);

  circles = new ArrayList<Circle>();

  byte[] data = loadBytes(TEST_FOLDER + "dna_quiroz.txt");
  println(data.length / 7);

  count = 0;
  index = 0;
  active = 0;
  fixed = -1;

  noStroke();
  for (int i = 0; i + 7 <= data.length; i += 7) {
    Circle circle = new Circle(Arrays.copyOfRange(data, i, i + 7));
    circles.add(circle);
  }

  loadPixels();
  for (int i = 0; i < width * height; i ++) {
    pixels[i] = color(0);
  }
}

void draw() {
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
      //fixed = i;
      //loadPixels();
      println(i + " " + active + " " + (active - i));
    }
  }

  count ++;
}