import cv2
import numpy as np
from PIL import Image
import random
import datetime
import os
from shutil import copyfile


MATE_PROBABILTY = 0.35
MUTATION_PROBABILITY = 0.45
HARD_MUTATION_PROBABILITY = 0.6

ADD_GEN = 5

POPULATION = 10
CIRCLES = 1

FILENAME = "monalisa"

ref = Image.open("../img/" + FILENAME + ".jpg")
ref = np.array(ref)

black = np.zeros((256, 256, 3), np.uint8)
copy = np.zeros((256, 256, 3), np.uint8)


def image(adn):
    i = 0
    img = np.array(copy)
    while i < len(adn):
        x = ord(adn[i])
        y = ord(adn[i + 1])
        radius = ord(adn[i + 2])
        alpha = ord(adn[i + 3])
        r = ord(adn[i + 4])
        g = ord(adn[i + 5])
        b = ord(adn[i + 6])

        cv2.circle(img, (x, y), radius, (r, g, b), -1)
        i += 7

    alpha = alpha / 255.0
    cv2.addWeighted(img, alpha, copy, 1.0 - alpha, 0, img)

    return img


def fitness(adn):
    black = image(adn)
    fit = 0
    for i in range(256):
        for j in range(256):
            for k in range(3):
                fit += abs(int(ref[i, j, k]) - int(black[i, j, k]))

    return fit


def select(samples):
    new_samples = []
    for i in range(len(samples)):
        candidates = random.sample(range(len(samples)), 2)
        if samples[candidates[0]][1] < samples[candidates[1]][1]:
            new_samples.append(samples[candidates[0]])
        else:
            new_samples.append(samples[candidates[1]])

    return new_samples


def mate(samples):
    new_samples = []
    for i in range(len(samples) // 2):
        dads = random.sample(samples, 2)
        prob = random.uniform(0.0, 1.0)
        if prob < MATE_PROBABILTY:
            cross = random.randint(0, len(dads[0]) - 1)
            son_a = dads[0][0][cross:] + dads[1][0][:cross]
            son_b = dads[1][0][cross:] + dads[0][0][:cross]

            fit_a = fitness(son_a)
            fit_b = fitness(son_b)
            new_samples.append((son_a, fit_a))
            new_samples.append((son_b, fit_b))

        else:
            new_samples.append(dads[0])
            new_samples.append(dads[1])

    return new_samples


def mutate(samples):
    new_samples = []
    for sample in samples:
        prob = random.uniform(0.0, 1.0)
        if prob <= MUTATION_PROBABILITY:
            hard = random.uniform(0.0, 1.0) <= HARD_MUTATION_PROBABILITY
            mutated = list(sample[0])
            for i in range(0, random.randint(1, len(sample[0]) / 2)):
                index = random.randint(0, len(sample[0]) - 1)
                if (hard):
                    mutated[index] = chr(random.randint(0, 255))
                else:
                    delta = random.randint(-16, 16)
                    mutated[index] = chr(max(0, min(255, ord(mutated[index]) + delta)))

            mutated = ''.join(mutated)
            fit = fitness(mutated)
            new_samples.append((mutated, fit))

        else:
            new_samples.append(sample)

    return new_samples


def create_population(circles):
    elite = None
    samples = []
    while len(samples) < POPULATION:
        sample = ""
        for i in range(7 * circles):
            sample += chr(random.randint(0, 255))

        fit = fitness(sample)
        samples.append((sample, fit))
        if elite is None or fit < elite[1]:
            elite = samples[-1]

    return elite, samples


def linear():
    circles = 1
    elite, samples = create_population(circles)

    add_elite = -1
    add_repetitions = 1

    date = datetime.datetime.now().strftime("%H.%M.%S")
    folder = "%s/%s/" % (FILENAME, date)
    os.makedirs(folder)
    copyfile(FILENAME + ".jpg", folder + FILENAME + ".jpg")

    k = 0
    while True:
        samples = select(samples)
        samples = mate(samples)
        samples = mutate(samples)

        pseudo = None
        for sample in samples:
            if pseudo is None or sample[1] < pseudo[1]:
                pseudo = sample

        if elite[1] < pseudo[1]:
            index = random.randint(0, len(samples) - 1)
            samples[index] = elite
        else:
            elite = pseudo

        if k % 10 == 0:
            best = image(elite[0])
            if elite[1] == add_elite:
                add_repetitions += 1
                if add_repetitions >= ADD_GEN:
                    global copy
                    copy = best
                    circles += 1
                    elite, samples = create_population(1)

            else:
                add_elite = elite[1]
                add_repetitions = 1

            img = Image.fromarray(best)
            percentage = 100.0 - (elite[1] * 100.0 / float(256 * 256 * 3 * 255))
            img.save("%s%04d (%d - %.2f).jpg" % (folder, k, elite[1], percentage))

        k += 1


linear()
