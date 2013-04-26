# L. Amber Wilcox-O'Hearn 2012
# SConstruct

# Ugly hack to avoid problem caused by ugly hack.
# See http://scons.tigris.org/issues/show_bug.cgi?id=2781
import sys
del sys.modules['pickle']

import os

import codecs, bz2, gzip, random, subprocess, json
from collections import defaultdict, Counter
from BackOffTrigramModel import BackOffTrigramModelPipe

from code.preprocessing import EssayRandomiser, GoldGenerator, StanfordTaggerPipe
from code.language_modelling import VocabularyCutter
from code.correction import VariationProposer, Corrector

def open_with_unicode(file_name, compression_type, mode):
    assert compression_type in [None, 'gzip', 'bzip2']
    assert mode in ['r', 'w']
    if compression_type == None:
        if mode == 'r':
            return codecs.getreader('utf-8')(open(file_name, mode))
        elif mode == 'w':
            return codecs.getwriter('utf-8')(open(file_name, mode))
    elif compression_type == 'gzip':
        if mode == 'r':
            return codecs.getreader('utf-8')(gzip.GzipFile(file_name, mode))
        elif mode == 'w':
            return codecs.getwriter('utf-8')(gzip.GzipFile(file_name, mode))
    elif compression_type == 'bzip2':
        if mode == 'r':
            return codecs.getreader('utf-8')(bz2.BZ2File(file_name, mode))
        elif mode == 'w':
            return codecs.getwriter('utf-8')(bz2.BZ2File(file_name, mode))

def randomise_essays(target, source, env):
    """
    target is a list of files corresponding to the training and
    development sets.  source is a single file of essays.
    """
    essay_file_obj = open_with_unicode(source[0].path, None, 'r')
    m2_file_obj = open_with_unicode(source[1].path, None, 'r')
    m2_5_file_obj = open_with_unicode(source[2].path, None, 'r')
    train_conll_file_obj = open_with_unicode(target[0].path, None, 'w')
    train_m2_file_obj = open_with_unicode(target[1].path, None, 'w')
    train_m2_5_file_obj = open_with_unicode(target[2].path, None, 'w')
    devel_conll_file_obj = open_with_unicode(target[3].path, None, 'w')
    devel_m2_file_obj = open_with_unicode(target[4].path, None, 'w')
    devel_m2_5_file_obj = open_with_unicode(target[5].path, None, 'w')
    rand_obj = random.Random(seed)
    er = EssayRandomiser.Randomiser(essay_file_obj, m2_file_obj, m2_5_file_obj, train_conll_file_obj, train_m2_file_obj, train_m2_5_file_obj, devel_conll_file_obj, devel_m2_file_obj, devel_m2_5_file_obj, rand_obj)
    er.randomise()
    return None

def training_m2_5_to_gold(target, source, env):
     """
     In addition to creating the gold file, we also make json'ed
     insertables and deletables files.
     """
     train_m2_5_file_obj = open_with_unicode(source[0].path, None, 'r')
     train_gold_file_obj = open_with_unicode(target[0].path, None, 'w')
     insertables_file_obj = open_with_unicode(target[1].path, None, 'w')
     deletables_file_obj = open_with_unicode(target[2].path, None, 'w')
     GoldGenerator.correct_file(train_m2_5_file_obj, train_gold_file_obj, insertables_file_obj, deletables_file_obj)
     return None


def create_vocabularies(target, source, env):
    """
    """
    train_gold_file_name = source[0].path
    srilm_ngram_counts = subprocess.Popen(['ngram-count', '-order', '1', '-tolower', '-text', train_gold_file_name, '-sort', '-write', data_directory + 'counts'])
    srilm_ngram_counts.wait()

    if vocabulary_sizes:
        unigram_counts_file_obj = open_with_unicode(data_directory + 'counts', None, 'r')
        size = vocabulary_sizes[0]
        vocabulary_file_name = data_directory + str(size) + 'K.vocab'
        assert os.path.normpath(target[0].path) == os.path.normpath(vocabulary_file_name), 'Target was: ' + target[0].path + '; Expected: ' + vocabulary_file_name
        vocabulary_file_obj = open_with_unicode(vocabulary_file_name, None, 'w')
        cutter = VocabularyCutter.VocabularyCutter(unigram_counts_file_obj, vocabulary_file_obj)
        cutter.cut_vocabulary(int(float(size)*1000))
        vocabulary_file_obj.close()
        base_vocabulary_file_obj = open_with_unicode(vocabulary_file_name, None, 'r')
        base_vocabulary = base_vocabulary_file_obj.readlines()

        for i in range(len(vocabulary_sizes))[1:]:
            size = vocabulary_sizes[i]
            vocabulary_file_name = data_directory + str(size) + 'K.vocab'
            assert target[i].path == vocabulary_file_name, 'Target was: ' + target[i].path + '; Expected: ' + vocabulary_file_name
            vocabulary_file_obj = open_with_unicode(vocabulary_file_name, None, 'w')
            for line in base_vocabulary[:int(float(size)*1000)]:
                vocabulary_file_obj.write(line)
    return None

def create_trigram_models(target, source, env):

    train_gold_file_name = source[0].path

    for i in range(len(vocabulary_sizes)):
        size = vocabulary_sizes[i]
        vocabulary_file_name = data_directory + str(size) + 'K.vocab'
        trigram_model_name = data_directory + 'trigram_model_' + str(size) + 'K.arpa'
        assert os.path.normpath(target[i].path) == os.path.normpath(trigram_model_name), target[i].path
        srilm_make_lm = subprocess.Popen(['ngram-count', '-vocab', vocabulary_file_name, '-tolower', '-unk', '-kndiscount3', '-debug', '2', '-text', train_gold_file_name, '-lm', trigram_model_name])
        srilm_make_lm.wait()

    return None

def get_pos_data(target, source, env):
    '''
    Creates pos_dictionary, POS training sets, pos_trigram_model.arpa, closed_class_pos_trigram_model.arpa
    '''

    tagger_pipe = StanfordTaggerPipe.StanfordTaggerPipe(data_directory + 'tagger.jar', module_path, data_directory + 'tagger')

    train_gold_file_obj = open_with_unicode(source[0].path, None, 'r')
    pos_training_file_obj = open_with_unicode(target[1].path, None, 'w')
    closed_class_pos_training_file_obj = open_with_unicode(target[2].path, None, 'w')
    pos_dictionary_set = defaultdict()

    print repr(get_pos_data), "POS tagging.  Progress dots per 100 sentences."
    line_number = 1
    for line in train_gold_file_obj:
        if not line_number % 100:
            print '.',
        words_and_tags = tagger_pipe.words_and_tags_list(line.strip())
        for w, t in words_and_tags:
            pos_training_file_obj.write(t + u' ')
            if t in closed_class_tags or w.lower() in AUX:
                closed_class_pos_training_file_obj.write(w.lower() + u' ')
            else:
                closed_class_pos_training_file_obj.write(t + u' ')
            if not pos_dictionary_set.has_key(t):
                pos_dictionary_set[t] = Counter()
            pos_dictionary_set[t][w.lower()] += 1

        pos_training_file_obj.write('\n')
        closed_class_pos_training_file_obj.write('\n')
        line_number += 1

    pos_dictionary = defaultdict(dict)
    for k,v in pos_dictionary_set.iteritems():
        pos_dictionary[k] = dict(v)

    pos_dictionary_file_obj = open_with_unicode(target[0].path, None, 'w')
    pos_dictionary_file_obj.write(json.dumps(pos_dictionary))

    return None

def make_pos_trigram_models(target, source, env):

    subprocess.Popen(['ngram-count', '-kndiscount3', '-text', source[0].path, '-lm', target[0].path])
    subprocess.Popen(['ngram-count', '-kndiscount3', 'unk', '-text', source[1].path, '-lm', target[1].path])

    return None

def correct(target, source, env):
    return real_correct(target, source, env)
    # return statprof_correct(target, source, env)
    #return cProfile_correct(target, source, env)

def cProfile_correct(target, source, env):
    import cProfile
    p = cProfile.Profile()
    p.enable()
    try:
        return real_correct(target, source, env)
    finally:
        p.disable()
        p.print_stats()

def statprof_correct(target, source, env):
    import statprof
    statprof.start()
    try:
        return real_correct(target, source, env)
    finally:
        statprof.stop()
        statprof.display()

def real_correct(target, source, env):

    pos_dictionary = json.load(open_with_unicode(source[1].path, None, 'r'))
    insertables =  json.load(open_with_unicode(source[2].path, None, 'r'))
    deletables =  json.load(open_with_unicode(source[3].path, None, 'r'))
    pos_tmpipe_obj = BackOffTrigramModelPipe.BackOffTMPipe('BackOffTrigramModelPipe', source[4].path)
    closed_class_tmpipe_obj = BackOffTrigramModelPipe.BackOffTMPipe('BackOffTrigramModelPipe', source[5].path)

    variation_proposers = []
    correctors = []
    corrections_file_objs = []
    tmpipe_objs = []
    for i in range(len(vocabulary_sizes)):
        tmpipe_obj = BackOffTrigramModelPipe.BackOffTMPipe('BackOffTrigramModelPipe', source[i+6].path)
        tmpipe_objs.append(tmpipe_obj)
        var_gen = VariationProposer.VariationProposer(pos_dictionary, tmpipe_obj, insertables, deletables)
        variation_proposers.append(var_gen)
        if pos_weight:
            correctors.append(Corrector.Corrector(tmpipe_obj, width, var_gen.generate_path_variations, error_probability, verbose=False, pos=pos_weight, pos_tmpipe_obj=pos_tmpipe_obj))
        else:
            correctors.append(Corrector.Corrector(tmpipe_obj, width, var_gen.generate_path_variations, error_probability, verbose=False, closed_class=closed_class_weight, closed_class_tmpipe=closed_class_tmpipe_obj, closed_class_tags=closed_class_tags, AUX=AUX))
        corrections_file_objs.append(open_with_unicode(target[i].path, None, 'w'))

    tagged_tokens = []
    for line in open_with_unicode(source[0].path, None, 'r'):
        if line == '\n':
            if tagged_tokens:
                for i in range(len(vocabulary_sizes)):
                    correction_tokens_list = [t[0] for t in correctors[i].get_correction(tagged_tokens)]
                    corrections_file_objs[i].write(' '.join(correction_tokens_list) + '\n')
                    corrections_file_objs[i].flush()
                tagged_tokens = []
        else:
            split_line = line.split()
            tagged_tokens.append( (split_line[4], split_line[5]) )

    for i in range(len(variation_proposers)):
        variation_proposers[i].print_cache_stats()

    return None

### begin copied in from the NUS M2 scorer "m2scorer.py" so that we could change it into a Python object instead of an operating system subprocess
from util import paragraphs

from levenshtein import edit_graph, set_weights, best_edit_seq_bf, levenshtein_matrix, transitive_arcs, equals_ignore_whitespace_casing, matchSeq, comp_p, comp_r, comp_f1

def batch_pre_rec_f1_ufo(output_ufo, candidates, sources, gold_edits, max_unchanged_words=2, ignore_whitespace_casing= False, verbose=False, very_verbose=False):
    assert len(candidates) == len(sources) == len(gold_edits), (len(candidates),  len(sources),  len(gold_edits))
    stat_correct = 0.0
    stat_proposed = 0.0
    stat_gold = 0.0
    for candidate, source, gold in zip(candidates, sources, gold_edits):
        candidate_tok = candidate.split()
        source_tok = source.split()
        lmatrix, backpointers = levenshtein_matrix(source_tok, candidate_tok)
        V, E, dist, edits = edit_graph(lmatrix, backpointers)
        if very_verbose:
            output_ufo.write(u"edit matrix: %s\n" % lmatrix)
            output_ufo.write(u"backpointers: %s\n" % backpointers)
            output_ufo.write(u"edits (w/o transitive arcs): %s\n" % edits)
        V, E, dist, edits = transitive_arcs(V, E, dist, edits, max_unchanged_words, very_verbose)
        dist = set_weights(E, dist, edits, gold, very_verbose)
        editSeq = best_edit_seq_bf(V, E, dist, edits, very_verbose)
        if very_verbose:
            output_ufo.write(u"Graph(V,E) =\n")
            output_ufo.write(u"V = %s\n" % V)
            output_ufo.write(u"E = %s\n" % E)
            output_ufo.write(u"edits (with transitive arcs): %s\n" % edits)
            output_ufo.write(u"dist() = %s\n" % dist)
            output_ufo.write(u"viterbi path = %s\n" % editSeq)
        if ignore_whitespace_casing:
            editSeq = filter(lambda x : not equals_ignore_whitespace_casing(x[2], x[3]), editSeq)
        correct = matchSeq(editSeq, gold, ignore_whitespace_casing)
        stat_correct += len(correct)
        stat_proposed += len(editSeq)
        stat_gold += len(gold)
        if verbose:
            output_ufo.write(u"SOURCE        : %s\n" % source)
            output_ufo.write(u"HYPOTHESIS    : %s\n" % candidate)
            output_ufo.write(u"EDIT SEQ      : %s\n" % editSeq)
            output_ufo.write(u"GOLD EDITS    : %s\n" % gold)
            output_ufo.write(u"CORRECT EDITS : %s\n" % correct)
            output_ufo.write(u"# correct     : %s\n" % stat_correct)
            output_ufo.write(u"# proposed    : %s\n" % stat_proposed)
            output_ufo.write(u"# gold        : %s\n" % stat_gold)
            output_ufo.write(u"precision     : %s\n" % comp_p(stat_correct, stat_proposed))
            output_ufo.write(u"recall        : %s\n" % comp_r(stat_correct, stat_gold))
            output_ufo.write(u"f1            : %s\n" % comp_f1(stat_correct, stat_proposed, stat_gold))
            output_ufo.write(u"-------------------------------------------\n")

    try:
        p  = stat_correct / stat_proposed
    except ZeroDivisionError:
        p = 1.0

    try:
        r  = stat_correct / stat_gold
    except ZeroDivisionError:
        r = 1.0
    try:
        f1  = 2.0 * p * r / (p+r)
    except ZeroDivisionError:
        f1 = 0.0
    if verbose:
        output_ufo.write(u"CORRECT EDITS  : %s\n" % stat_correct)
        output_ufo.write(u"PROPOSED EDITS : %s\n" % stat_proposed)
        output_ufo.write(u"GOLD EDITS     : %s\n" % stat_gold)
        output_ufo.write(u"P = %s\n" % p)
        output_ufo.write(u"R = %s\n" % r)
        output_ufo.write(u"F1 = %s\n" % f1)
    return (p, r, f1)

class M2Scorer:
    def __init__(self, gold_ufo, verbose=False, max_unchanged_words=2, ignore_whitespace_casing=False, very_verbose=False):
        self.verbose = verbose
        self.max_unchanged_words = max_unchanged_words
        self.ignore_whitespace_casing = ignore_whitespace_casing
        self.very_verbose = very_verbose 

        self.source_sentences = []
        self.gold_edits = []

        self._load_annotation(gold_ufo)

    def _load_annotation(self, gold_ufo):
        for item in paragraphs(gold_ufo):
            item = item.splitlines(False)
            sentence = [line[2:].strip() for line in item if line.startswith('S ')]
            assert sentence != []
            annotation = []
            for line in item[1:]:
                if line.startswith('I ') or line.startswith('S '):
                    continue
                assert line.startswith('A ')
                line = line[2:]

                fields = line.split('|||')
                start_offset = int(fields[0].split()[0])
                end_offset = int(fields[0].split()[1])
                corrections =  [c.strip() if c != '-NONE-' else '' for c in fields[2].split('||')]
                # NOTE: start and end are *token* offsets
                original = ' '.join(' '.join(sentence).split()[start_offset:end_offset])
                annotation.append((start_offset, end_offset, original, corrections))
            tok_offset = 0
            for this_sentence in sentence:
                tok_offset += len(this_sentence.split())
                this_edits = [edit for edit in annotation if edit[0] <= tok_offset and edit[1] <= tok_offset]
                self.source_sentences.append(this_sentence)
                self.gold_edits.append(this_edits)

    def score(self, system_ufo, output_ufo):
        system_sentences = [line.strip() for line in system_ufo]

        p, r, f1 = batch_pre_rec_f1_ufo(output_ufo, system_sentences, self.source_sentences, self.gold_edits, self.max_unchanged_words, self.ignore_whitespace_casing, self.verbose, self.very_verbose)

        output_ufo.write(u"Precision   : %.4f\n" % p)
        output_ufo.write(u"Recall      : %.4f\n" % r)
        output_ufo.write(u"F1          : %.4f\n" % f1)
### end copied in from the NUS M2 scorer "m2scorer.py" so that we could change it into a Python object instead of an operating system subprocess

def score_corrections(target, source, env):
    return pythonic_score_corrections(target, source, env)
    # return subprocess_score_corrections(target, source, env)

def pythonic_score_corrections(target, source, env):
    gold_fname = os.path.join(data_directory, 'development_set_m2_5')

    scorer = M2Scorer(open_with_unicode(gold_fname, None, 'r'), verbose=True, max_unchanged_words=7)

    for i in range(len(vocabulary_sizes)):
        print source[i].path
        res_ufo = open_with_unicode(source[i].path, None, 'r')
        score_file_obj = open_with_unicode(target[i].path, None, 'w')
        scorer.score(res_ufo, score_file_obj)

def subprocess_score_corrections(target, source, env):

    for i in range(len(vocabulary_sizes)):
        score_file_obj = open_with_unicode(target[i].path, None, 'w')

        print source[i].path
        scorer = subprocess.Popen([sys.executable, data_directory + 'm2scorer.py', '-v', '--max_unchanged_words', '7', source[i].path, data_directory + 'development_set_m2_5'], stdout=score_file_obj)
        scorer.communicate(None)

    return None


# Hard coding this for now... TODO make variables
module_path = 'edu.stanford.nlp.tagger.maxent.MaxentTagger'
closed_class_tags = ['IN', 'DT', 'TO', 'MD']
AUX = ['be', 'is', 'are', 'were', 'was', 'been', 'being', 'have', 'has', 'had', 'having', 'do', 'does', 'did', 'get', 'got', 'getting']

# Get commandline configuration:

data_directory = ''
vocabulary_sizes = []
TEST = False

try:
    data_directory = [x[1] for x in ARGLIST if x[0] == "data_directory"][0] + '/'
except:
    print "Usage: scons data_directory=DIR variables target"
    raise Exception

try:
    seed = int([x[1] for x in ARGLIST if x[0] == 'seed'][0])
except:
    seed = 7

try:
    error_probability = float([x[1] for x in ARGLIST if x[0] == 'error_probability'][0])
except:
    error_probability = -1.3

try:
    width = int([x[1] for x in ARGLIST if x[0] == 'width'][0])
except:
    width = 9

try:
    pos_weight = float([x[1] for x in ARGLIST if x[0] == 'pos_weight'][0])
except:
    pos_weight = 0

try:
    closed_class_weight = float([x[1] for x in ARGLIST if x[0] == 'closed_class_weight'][0])
except:
    closed_class_weight = 0

if [x for x in ARGLIST if x[0] == "test"]:
    TEST = True
    vocabulary_sizes = [0.1, 0.5]
    pos_weight = .5
else:
    for key, value in ARGLIST:
        if key == "vocabulary_size":
            vocabulary_sizes.append(value)

assert not (pos_weight and closed_class_weight), "Choose either pos_weight or closed_class_weight."

learning_sets_builder = Builder(action = randomise_essays)
training_gold_builder = Builder(action = training_m2_5_to_gold)
vocabulary_files_builder = Builder(action = create_vocabularies)
trigram_models_builder = Builder(action = create_trigram_models)
pos_data_builder = Builder(action = get_pos_data)
pos_trigram_model_builder = Builder(action = make_pos_trigram_models)
corrections_builder = Builder(action = correct)
scores_builder = Builder(action = score_corrections)

env = Environment(BUILDERS = {'learning_sets' : learning_sets_builder, 'training_gold': training_gold_builder, 'vocabulary_files': vocabulary_files_builder, 'trigram_models' : trigram_models_builder, 'pos_data' : pos_data_builder, 'pos_trigram_models' : pos_trigram_model_builder, 'corrections': corrections_builder, 'scores': scores_builder})

env.learning_sets([data_directory + set_name for set_name in ['training_set', 'training_set_m2', 'training_set_m2_5', 'development_set', 'development_set_m2', 'development_set_m2_5']], [data_directory + 'corpus', data_directory + 'm2', data_directory + 'm2_5'])

env.Alias('learning_sets', [data_directory + set_name for set_name in ['training_set', 'training_set_m2', 'training_set_m2_5', 'development_set', 'development_set_m2', 'development_set_m2_5']])

env.training_gold([data_directory + 'training_set_gold', data_directory + 'insertables', data_directory + 'deletables'], [data_directory + 'training_set_m2_5'])

env.Alias('training_gold', [data_directory + 'training_set_gold', data_directory + 'insertables', data_directory + 'deletables'])

env.vocabulary_files([data_directory + str(size) + 'K.vocab' for size in vocabulary_sizes], [data_directory + 'training_set_gold'])

env.Alias('vocabulary_files', [data_directory + str(size) + 'K.vocab' for size in vocabulary_sizes])

env.trigram_models([data_directory + 'trigram_model_' + str(size) + 'K.arpa' for size in vocabulary_sizes], [data_directory + 'training_set_gold'] + [data_directory + str(size) + 'K.vocab' for size in vocabulary_sizes])

env.Alias('trigram_models', [data_directory + 'trigram_model_' + str(size) + 'K.arpa' for size in vocabulary_sizes])

env.pos_data([data_directory + target for target in ["pos_dictionary", "pos_training_set", 'closed_class_pos_training_set']], [data_directory + "training_set_gold"])

env.Alias("pos_data", [data_directory + target for target in ["pos_dictionary", "pos_training_set", 'closed_class_pos_training_set']])

env.pos_trigram_models([data_directory + 'pos_trigram_model.arpa', data_directory + 'closed_class_pos_trigram_model.arpa'], [data_directory + 'pos_training_set', data_directory + 'closed_class_pos_training_set'])

env.Alias('pos_trigram_models', [data_directory + 'pos_trigram_model.arpa', data_directory + 'closed_class_pos_trigram_model.arpa'])

env.corrections([data_directory + 'corrections_trigram_model_size_' + str(size) + 'K_pos_weight_' + str(pos_weight) if pos_weight else data_directory + 'corrections_trigram_model_size_' + str(size) + 'K_closed_class_weight_' + str(closed_class_weight) for size in vocabulary_sizes], [data_directory + 'development_set', data_directory + 'pos_dictionary', data_directory + 'insertables', data_directory + 'deletables', data_directory + 'pos_trigram_model.arpa', data_directory + 'closed_class_pos_trigram_model.arpa'] + [data_directory + 'trigram_model_' + str(size) + 'K.arpa' for size in vocabulary_sizes])

env.Alias('corrections', [data_directory + 'corrections_trigram_model_size_' + str(size) + 'K_pos_weight_' + str(pos_weight) if pos_weight else data_directory + 'corrections_trigram_model_size_' +str(closed_class_weight) for size in vocabulary_sizes])

env.scores([data_directory + 'scores_trigram_model_size_' + str(size) + 'K_pos_weight_' + str(pos_weight) if pos_weight else data_directory + 'scores_trigram_model_size_' + str(size) + 'K_closed_class_weight_' + str(closed_class_weight) for size in vocabulary_sizes], [data_directory + 'corrections_trigram_model_size_' + str(size) + 'K_pos_weight_' + str(pos_weight) if pos_weight else data_directory + 'corrections_trigram_model_size_' + str(size) + 'K_closed_class_weight_' + str(closed_class_weight) for size in vocabulary_sizes])

env.Alias('scores', [data_directory + 'scores_trigram_model_size_' + str(size) + 'K_pos_weight_' + str(pos_weight) if pos_weight else data_directory + 'scores_trigram_model_size_' + str(size) + 'K_closed_class_weight_' + str(closed_class_weight) for size in vocabulary_sizes])
