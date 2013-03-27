# L. Amber Wilcox-O'Hearn 2013
# Corrector.py

def beam_search(sentence, width, prob_of_err_func, path_prob_func, variation_generator):
    """
    For each token in the sentence, put all variations on every
    existing path on the beam.  Sort and prune. At the end, return the
    best path.
    """

    beam = [(0, [])]
    for i in range(len(sentence)):
        new_beam = []
        for path in beam:
            log_prob, tokens = path
            with_original = tokens + [sentence[i]]
            new_beam.append((path_prob_func(log_prob, with_original), with_original))
            for path_variation in variation_generator(tokens + [sentence[i]]):
                new_beam.append((path_prob_func(log_prob, path_variation) + prob_of_err_func(path_variation), path_variation))

        new_beam.sort()
        beam = new_beam[-width:]

    return beam[-1][1]