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

#define FILENAME "monalisa"


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
		return 0;
	}
};

void select(vector<Circle *> &samples) {
	int n = samples.size();
	vector<Circle *> newSamples(n);

	for (int i = 0; i < n; i ++) {
		int a = rand() % n;
		int b = rand() % n;

		// Select the fittest
		newSamples[i] = ((*samples[a]).fitness < (*samples[b]).fitness) ? samples[a] : samples[b];
	}

	samples = newSamples;
}

void mate(vector<Circle *> &samples) {
	int n = samples.size();
	vector<Circle *> newSamples(n);

	for (int i = 0; i < n / 2; i ++) {
		Circle * dadA = samples[rand() % n];
		Circle * dadB = samples[rand() % n];

		float prob = rand() / RAND_MAX;
		if (prob < MATE_RATE) {
			int l = (*dadA).dna.length();
			int crossPoint = rand() % l;
			string sonA = (*dadA).dna.substr(0, crossPoint) + (*dadB).dna.substr(crossPoint, l);
			string sonB = (*dadB).dna.substr(0, crossPoint) + (*dadA).dna.substr(crossPoint, l);

			newSamples[i] = new Circle(sonA);
			newSamples[i + 1] = new Circle(sonB);

		} else {
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
		for (int j = 0; j < CIRCLES_PER_GEN; j ++) {
			dna += (char)(rand() % 256);
		}

		// Save elite
		Circle *circle = new Circle(dna);
		if (elite == nullptr || (*circle).fitness < (*elite).fitness) {
			elite = circle;
		}
	}

	return samples;
}

int main() {
	// Load reference image
	Mat reference;// = imread(FILENAME, CV_LOAD_IMAGE_COLOR);

	// Create folder structure with current date
	char buffer[50];
	time_t rawtime = time(NULL);
	strftime(buffer, 50, "%H.%M.%S", localtime(&rawtime));
	string date(buffer);
	string folder = FILENAME + (string)"\\" + date + "\\";
	_mkdir(folder.c_str());

	// Copy reference image
	ifstream src(FILENAME + (string)".jpg", ios::binary);
	ofstream dest(folder + FILENAME + (string)".jpg", ios::binary);

	Circle *elite = nullptr;
	vector<Circle *> population = createPopulation(elite);

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
		}

		gen ++;
	}
}
