#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <stack>
#include <vector>
#include <string>
#include <queue>
#include <map>
#include <math.h>
#include <time.h>
#include <ctime>
#include <direct.h>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


#define MATE_RATE 0.35
#define MUTATION_RATE 0.45
#define HARD_MUTATION_RATE 0.6

#define POPULATION 10
#define CIRCLES_PER_GEN 1
#define REP_TOLERANCE 5

#define IMAGE_FOLDER "../../../img/"
#define TEST_FOLDER "../../../test/"

#define FILENAME "quiroz"


Mat reference, copyImage;


class Circle {
public:
	int fitness;
	string dna;

	Circle() { }

	Circle(string _dna) {
		dna = _dna;
		fitness = calculateFitness();
	}

	int calculateFitness() {
		Mat black = createImage();
		int fitness = 0;

		// Add delta for each pixel, on all three RGB chanels
		for (int i = 0; i < 256; i ++) {
			for (int j = 0; j < 256; j ++) {
				for (int k = 0; k < 3; k ++) {
					fitness += abs(reference.at<Vec3b>(i, j)[k] - black.at<Vec3b>(i, j)[k]);
				}
			}
		}

		return fitness;
	}

	Mat createImage() {
		Mat img = copyImage.clone();

		for (int i = 0; i < dna.length();) {
			int x = (unsigned char)dna[i ++];
			int y = (unsigned char)dna[i ++];
			int radius = (unsigned char)dna[i ++];
			double alpha = (unsigned char)dna[i ++] / 255.0;
			int r = (unsigned char)dna[i ++];
			int g = (unsigned char)dna[i ++];
			int b = (unsigned char)dna[i ++];

			// Add circle to current image
			Mat blank = img.clone();
			circle(blank, Point(x, y), radius, Scalar(b, g, r), -1);
			addWeighted(blank, alpha, img, 1.0 - alpha, 0, img);
		}
		
		return img;
	}
};

void select(vector<Circle *> &samples) {
	int n = samples.size();
	vector<Circle *> newSamples(n);

	for (int i = 0; i < n; i ++) {
		// Select the fittest
		int a = rand() % n;
		int b = rand() % n;
		newSamples[i] = ((*samples[a]).fitness < (*samples[b]).fitness) ? samples[a] : samples[b];
	}

	samples = newSamples;
}

void mate(vector<Circle *> &samples) {
	int n = samples.size();
	vector<Circle *> newSamples(n);

	for (int i = 0; i < n; i += 2) {
		Circle * dadA = samples[rand() % n];
		Circle * dadB = samples[rand() % n];

		float prob = (double)rand() / RAND_MAX;
		if (prob < MATE_RATE) {
			// Mate dads, merging from a random cross point
			int l = (*dadA).dna.length();
			int crossPoint = rand() % l;
			string sonA = (*dadA).dna.substr(0, crossPoint) + (*dadB).dna.substr(crossPoint, l);
			string sonB = (*dadB).dna.substr(0, crossPoint) + (*dadA).dna.substr(crossPoint, l);

			newSamples[i] = new Circle(sonA);
			newSamples[i + 1] = new Circle(sonB);

		} else {
			// No mutation is necessary, pass both dads unchanged
			newSamples[i] = dadA;
			newSamples[i + 1] = dadB;
		}
	}

	samples = newSamples;
}

void mutate(vector<Circle *> &samples) {
	int n = samples.size();
	vector<Circle *> newSamples(n);

	for (int i = 0; i < n; i ++) {
		float prob = rand() / RAND_MAX;
		if (prob < MUTATION_RATE) {
			bool hard = (rand() / RAND_MAX < HARD_MUTATION_RATE);
			string mutated((*samples[i]).dna);

			// Mutate DNA from sample
			int mutationCount = rand() % (mutated.length() / 2);
			for (int i = 0; i < mutationCount; i ++) {
				int index = rand() % mutated.length();
				
				if (hard) {
					mutated[index] = (char)(rand() % 256);

				} else {
					int delta = (rand() % 32) - 16;
					mutated[index] = (char)max(0, min(255, mutated[index] + delta));
				}
			}

			// Add new mutated sample
			newSamples[i] = new Circle(mutated);

		} else {
			newSamples[i] = samples[i];
		}
	}

	samples = newSamples;
}

vector<Circle *> createPopulation(Circle *&elite) {
	vector<Circle *> samples(POPULATION);

	for (int i = 0; i < samples.size(); i ++) {
		// Create random dna
		string dna = "";
		for (int j = 0; j < 7 * CIRCLES_PER_GEN; j ++) {
			dna += (unsigned char)(rand() % 256);
		}

		// Keep track of elite sample
		Circle *circle = new Circle(dna);
		if (elite == nullptr || (*circle).fitness < (*elite).fitness) {
			elite = circle;
		}

		samples[i] = circle;
	}

	return samples;
}

int main() {
	// Give new seed to random generator
	srand(time(NULL));

	// Load reference image
	reference = imread(IMAGE_FOLDER FILENAME ".jpg", CV_LOAD_IMAGE_COLOR);
	copyImage = Mat(256, 256, CV_8UC3, Scalar(0, 0, 0));

	// Create folder structure with current date
	char buffer[50];
	time_t rawtime = time(NULL);
	strftime(buffer, 50, "%H.%M.%S", localtime(&rawtime));
	string date(buffer);
	string folder = TEST_FOLDER FILENAME "/" + date + "/";
	_mkdir(TEST_FOLDER);
	_mkdir(TEST_FOLDER FILENAME);
	_mkdir(folder.c_str());

	// Copy reference image file
	imwrite(folder + FILENAME + (string)".jpg", reference);

	// Create DNA file
	ofstream file;
	file.open(TEST_FOLDER "dna.txt");
	file.close();

	// Create population
	Circle *elite = nullptr;
	vector<Circle *> population = createPopulation(elite);

	int currentEliteFitness = (*elite).fitness;
	int repetitions = 0;
	int gen = 0;
	while (true) {
		select(population);
		mate(population);
		mutate(population);

		// Select elite from new population
		Circle *pseudoElite = nullptr;
		for (Circle *sample : population) {
			if (pseudoElite == nullptr || (*sample).fitness < (*pseudoElite).fitness) {
				pseudoElite = sample;
			}
		}

		if ((*elite).fitness < (*pseudoElite).fitness) {
			// Re-introduce alpha elite to the population
			int index = rand() % population.size();
			population[index] = elite;

		} else {
			// Keep track of new alpha elite
			elite = pseudoElite;
		}

		if (gen % 10 == 0) {
			// Save new snapshot
			float percentage = 100.0 - ((*elite).fitness * 100.0 / (float)(256 * 256 * 3 * 255));
			imwrite(folder + to_string(gen) + " (" + to_string((*elite).fitness) + " - " + to_string(percentage) + ").jpg", copyImage);

			// Save elite image and recreate population whenever the
			// current elite fitness repeats for a fixed amount of generations
			if (currentEliteFitness == (*elite).fitness) {
				repetitions ++;

				if (repetitions >= REP_TOLERANCE) {
					// Elite repeated enough times, save dna
					int i = 0;
					ofstream file;
					file.open(TEST_FOLDER "dna.txt", ofstream::ate|ofstream::app|ofstream::binary);
					file << (unsigned char)(*elite).dna[i ++];
					file << (unsigned char)(*elite).dna[i ++];
					file << (unsigned char)(*elite).dna[i ++];
					file << (unsigned char)(*elite).dna[i ++];
					file << (unsigned char)(*elite).dna[i ++];
					file << (unsigned char)(*elite).dna[i ++];
					file << (unsigned char)(*elite).dna[i ++];
					file.close();

					// Save image and create new population
					copyImage = (*elite).createImage();
					elite = nullptr;
					population = createPopulation(elite);
				}

			} else {
				currentEliteFitness = (*elite).fitness;
				repetitions = 1;
			}
		}

		gen ++;
	}
}
