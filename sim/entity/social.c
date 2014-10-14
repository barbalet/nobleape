/****************************************************************

 social.c

 =============================================================

 Copyright 1996-2014 Tom Barbalet. All rights reserved.

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or
 sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

 This software and Noble Ape are a continuing work of Tom Barbalet,
 begun on 13 June 1996. No apes or cats were harmed in the writing
 of this software.

 ****************************************************************/

/*! \file   social.c
 *  \brief  This handles social interactions and management of the social graph
 */

#include "entity.h"
#include "entity_internal.h"

/** Status preference */

#define GENE_STATUS_PREFERENCE(gene)        GENE_VAL_REG(gene, 15, 12, 10, 1)

/** Pigmentation preference */

#define GENE_PIGMENTATION_PREFERENCE(gene)  GENE_VAL_REG(gene, 5, 3, 11, 4)

/** mating preference for height */

#define GENE_HEIGHT_PREFERENCE(gene)        GENE_VAL_REG(gene, 9, 8, 14, 10)

/** mating preference for frame */

#define GENE_FRAME_PREFERENCE(gene)         GENE_VAL_REG(gene, 9, 0, 8, 2)

/** mating preference for hair length */

#define GENE_HAIR_PREFERENCE(gene)          GENE_VAL_REG(gene, 10, 7, 14, 15)

/** Groom */

#define GENE_GROOM(gene)                    GENE_VAL_REG(gene, 14, 2, 5, 10)

/** Aggression */

#define GENE_AGGRESSION(gene)               GENE_VAL_REG(gene, 11, 3, 5, 0)

/** Mate bond */

#define GENE_MATE_BOND(gene)                GENE_VAL_REG(gene, 10, 2, 4, 0)

/** Degree of aversion to incest */

#define GENE_INCEST_AVERSION(gene)          GENE_VAL_REG(gene, 10, 8, 4, 9)

/** Latent energy use */

#define GENE_LATENT_ENERGY_USE(gene)        GENE_VAL_REG(gene, 14, 3, 6, 10)

#ifdef FEATURE_SET

static void noble_feature_copy(noble_feature * to, noble_feature * from)
{
    to->type = from->type;
    to->value = from->value;
    to->frequency = from->frequency;
}

static void noble_feature_set(noble_feature * to, n_byte feature_type, n_byte2 feature_value)
{
    to->type =  (n_byte)feature_type;
    to->value =  (n_byte2)feature_value;
    to->frequency = (n_byte2)1;
}

/**
 * @brief Returns the array index of a given feature type within a set
 * @param s A set of features
 * @param feature_type
 * @return array index of a given feature type within a set
 */
static n_int noble_featureset_feature_index(noble_featureset * s,
                                            n_byte feature_type)
{
    n_int i=0;

    while (i < s->feature_number)
    {
        if (s->features[i].type >= feature_type)
        {
            break;
        }
        i++;
    }
    if (i == s->feature_number)
    {
        return -1;
    }
    return i;
}

/**
 * @brief Normalises the feature frequencies within a set
 * @param s a set of features
 */
static void noble_featureset_normalise_feature_frequencies(noble_featureset *s)
{
    n_uint i, tot=0;
    n_uint max = MAX_FEATURE_FREQUENCY>>1;

    /** get the total frequency count */
    for (i = 0; i < s->feature_number; i++)
    {
        tot += (n_uint)s->features[i].frequency;
    }
    
    if (tot == 0) tot = 1;
    
    for (i = 0; i < s->feature_number; i++)
    {
        s->features[i].frequency = (n_byte2)((n_uint)s->features[i].frequency * max / tot);
    }
}

/**
 * @brief Adds a feature to the given set
 * @param s Pointer to the feature set
 * @param feature_type the type of feature
 * @param feature_value value of the feature
 * @return 0 on success, -1 otherwise
 */
static n_int noble_featureset_update(noble_featureset * s,
                                     n_byte feature_type,
                                     n_int feature_value)
{
    /** get the index of the feature within the array */
    n_int feature_index = noble_featureset_feature_index(s, feature_type);
    n_byte2 min;
    n_int i,j;

    if (s->features[feature_index].type == (n_byte)feature_type)
    {
        /** alter the value associated with an existing feature type */
        s->features[feature_index].value = (n_byte2)feature_value;
        s->features[feature_index].frequency++;
        /** normalise the feature frequencies to prevent them
            from going out of bounds */
        if (s->features[feature_index].frequency > MAX_FEATURE_FREQUENCY)
        {
            noble_featureset_normalise_feature_frequencies(s);
        }
        return 0;
    }
    else
    {
        if (s->feature_number < MAX_FEATURESET_SIZE)
        {
            /** add a new feature type to the array */
            if (s->feature_number > 1)
            {
                for (i = (n_int)s->feature_number-1; i >= (n_int)feature_index; i--)
                {
                    noble_feature_copy(&(s->features[i+1]), &(s->features[i]));
                }
            }

            i = feature_index;
            s->feature_number++;
            noble_feature_set(&(s->features[i]), (n_byte)feature_type, (n_byte2)feature_value);
            return 0;
        }
        else
        {
            /** pick the least frequent feature and replace it */
            min = s->features[0].frequency;
            feature_index = 0;
            for (i = 1; i < (n_int)s->feature_number; i++)
            {
                if (s->features[i].frequency < min)
                {
                    min = s->features[i].frequency;
                    feature_index = i;
                }
            }
            /** re-sort */
            j = 0;
            for (i = 0; i < (n_int)s->feature_number; i++)
            {
                if (s->features[i].type >= (n_byte)feature_type)
                {
                    j = i;
                    break;
                }
            }
            for (i = (n_int)feature_index; i > j; i--)
            {
                noble_feature_copy(&(s->features[i]), &(s->features[i-1]));
            }
            
            
            noble_feature_set(&(s->features[j]), (n_byte)feature_type, (n_byte2)feature_value);
            
            for (i = 0; i < (n_int)s->feature_number; i++)
            {
                for (j = i+1; j < (n_int)s->feature_number; j++)
                {
                    if (s->features[j].type < s->features[i].type)
                    {
                        feature_type = s->features[i].type;
                        s->features[i].type = s->features[j].type;
                        s->features[j].type = (n_byte)feature_type;
                    }
                }
            }
        }
    }
    return -1;
}

/**
 * @brief returns a threshold which is used to determine
 *        whether two features of the same type have a matching value
 * @param feature_type
 * @return
 */
static n_int featureset_match_threshold(n_byte feature_type)
{
    if (feature_type == FEATURESET_TERRITORY) return 1;
    return 2;
}

/**
 * @brief Normalises the number of observations for each stereotype
 * @param local_being Pointer to the being
 */
static void social_normalise_stereotype_observations(
    noble_being * local_being)
{
    noble_social * graph;
    n_uint i, tot=0;
    noble_featureset * s;
    n_uint max = MAX_FEATURESET_OBSERVATIONS>>1;

    /** Get the social graph */
    graph = being_social(local_being);

    if (graph==0) return;

    for (i = SOCIAL_SIZE_BEINGS; i < SOCIAL_SIZE; i++)
    {
        if (!SOCIAL_GRAPH_ENTRY_EMPTY(graph,i))
        {
            s = &graph[i].classification;
            tot += (n_uint)s->observations;
        }
    }

    if (tot == 0) return;

    for (i = SOCIAL_SIZE_BEINGS; i < SOCIAL_SIZE; i++)
    {
        if (!SOCIAL_GRAPH_ENTRY_EMPTY(graph,i))
        {
            s = &graph[i].classification;
            s->observations =
                (n_byte2)((n_uint)s->observations * max / tot);
        }
    }
}

/**
 * @brief Returns the social graph array index of the closest matching
 *        stereotype to the met being
 * @param meeter_being Pointer to the being doing the meeting
 * @param social_graph_index Social graph index of the being which was met
 * @return Social graph array index of the closest stereotype
 *         or -1 of there are no stereotypes
 */
static n_int social_get_stereotype(
    noble_being * meeter_being,
    n_int social_graph_index)
{
    n_int i,j,diff,dv,index,hits,min=0,result=-1;
    n_byte normalise_features;
    noble_social * meeter_social_graph;
    noble_featureset * s1, * s2;

    /** Get the social graph for the being doing the meeting */
    meeter_social_graph = being_social(meeter_being);

    if (meeter_social_graph==0) return -1;

    /** get the observed feature set for the met being */
    s2 = &meeter_social_graph[social_graph_index].classification;

    /** the upper range of social graph entries between
        SOCIAL_SIZE_BEINGS and SOCIAL_SIZE contains
        abstract beings or stereotypes */
    for (i = SOCIAL_SIZE_BEINGS; i < SOCIAL_SIZE; i++)
    {
        if (!SOCIAL_GRAPH_ENTRY_EMPTY(meeter_social_graph,i))
        {
            /** get the feature set for the stereotype */
            s1 = &meeter_social_graph[i].classification;
            normalise_features = 0;
            diff = 0;
            hits = 0;
            /** for every feature within the stereotype */
            for (j = 0; j < s1->feature_number; j++)
            {
                /** does this feature exist for the met being? */
                index = noble_featureset_feature_index(s2, s1->features[j].type);
                if (index > -1)
                {
                    hits++;
                    /** difference between the feature values */
                    dv = (n_int)s1->features[j].value -
                         (n_int)s2->features[index].value;
                    if (dv < 0) dv = -dv;

                    /** update the total difference between the stereotype
                        and the met being */
                    diff += dv;

                    /** does the stereotype feature match the met being feature?
                        if so then increment the observation frequency */
                    if (dv < featureset_match_threshold(s1->features[j].type))
                    {
                        /** increment the frequency of stereotype features */
                        s1->features[j].frequency++;
                        if (s1->features[j].frequency > MAX_FEATURE_FREQUENCY)
                        {
                            normalise_features = 1;
                        }
                    }
                }
            }
            /** if all stereotype features were matched
                and the match was better than the best found */
            if (hits == s1->feature_number)
            {
                if ((result == -1) || (diff < min))
                {
                    min = diff;
                    result  = i;
                }
                /** increment the number of times when this
                    stereotype was fully matched */
                s1->observations++;
                if (s1->observations > MAX_FEATURESET_OBSERVATIONS)
                {
                    social_normalise_stereotype_observations(meeter_being);
                }
            }

            /** normalise the stereotype feature frequencies if necessary */
            if (normalise_features == 1)
            {
                noble_featureset_normalise_feature_frequencies(s1);
            }
        }
    }
    return result;
}

/**
 * @brief When one being meets another remember the observable features
 * @param meeter_being the being foing the meeting
 * @param met_being the being which was met
 * @param social_graph_index index within the meeters social graph of the met being
 */
static void social_meet_update_features(
    noble_being * meeter_being,
    noble_being * met_being,
    n_int social_graph_index)
{
    noble_social * meeter_social_graph;
#ifdef TERRITORY_ON
    n_uint idx;
#endif

    /** Get the social graph for the being doing the meeting */
    meeter_social_graph = being_social(meeter_being);

    if (meeter_social_graph==0) return;

    /** Note: perhaps not all features should be observed at once.
        This should maybe be under attentional control.
        Also observations should perhaps include learned biases
        from existing stereotypes */

    noble_featureset_update(&meeter_social_graph[social_graph_index].classification,
                            FEATURESET_PIGMENTATION,
                            GENE_PIGMENTATION(being_genetics(met_being)));

    noble_featureset_update(&meeter_social_graph[social_graph_index].classification,
                            FEATURESET_HAIR,
                            GENE_HAIR(being_genetics(met_being)));

    noble_featureset_update(&meeter_social_graph[social_graph_index].classification,
                            FEATURESET_HEIGHT,
                            GET_H(met_being));

    noble_featureset_update(&meeter_social_graph[social_graph_index].classification,
                            FEATURESET_FAT,
                            GET_BODY_FAT(met_being));

    noble_featureset_update(&meeter_social_graph[social_graph_index].classification,
                            FEATURESET_EYE_SHAPE,
                            GENE_EYE_SHAPE(being_genetics(met_being)));

    noble_featureset_update(&meeter_social_graph[social_graph_index].classification,
                            FEATURESET_EYE_COLOR,
                            GENE_EYE_COLOR(being_genetics(met_being)));

    noble_featureset_update(&meeter_social_graph[social_graph_index].classification,
                            FEATURESET_EYE_SEPARATION,
                            GENE_EYE_SEPARATION(being_genetics(met_being)));

    noble_featureset_update(&meeter_social_graph[social_graph_index].classification,
                            FEATURESET_NOSE_SHAPE,
                            GENE_NOSE_SHAPE(being_genetics(met_being)));

    noble_featureset_update(&meeter_social_graph[social_graph_index].classification,
                            FEATURESET_EAR_SHAPE,
                            GENE_EAR_SHAPE(being_genetics(met_being)));

    noble_featureset_update(&meeter_social_graph[social_graph_index].classification,
                            FEATURESET_EYEBROW_SHAPE,
                            GENE_EYEBROW_SHAPE(being_genetics(met_being)));

    noble_featureset_update(&meeter_social_graph[social_graph_index].classification,
                            FEATURESET_MOUTH_SHAPE,
                            GENE_MOUTH_SHAPE(being_genetics(met_being)));

#ifdef TERRITORY_ON
    idx = APESPACE_TO_TERRITORY(being_location_y(meeter_being))*TERRITORY_DIMENSION +
          APESPACE_TO_TERRITORY(being_location_x(meeter_being));

    noble_featureset_update(&meeter_social_graph[social_graph_index].classification,
                            FEATURESET_TERRITORY,
                            meeter_being->territory[idx].name);
#endif
}

#endif

/**
 * @brief Returns a string for the name of the ape in the given social graph array index.
 * @param local_sim Pointer to the simulation
 * @param localbeing Pinter to the ape
 * @param social_graph_index Array index within the social graph
 * @param met BEING_MEETER=return name for the meeter, BEING_MET=return name for the met
 * @param name Returned ape name
 */
void social_graph_link_name(
    noble_simulation * local_sim,
    noble_being * local_being,
    n_int social_graph_index,
    n_byte met,
    n_string name)
{
    noble_social * local_social_graph;

    /** Get the social graph for the being */
    local_social_graph = being_social(local_being);

    if (local_social_graph==0) return;

    switch(local_social_graph[social_graph_index].entity_type)
    {
    case ENTITY_BEING:
    {
        being_name_byte2(local_social_graph[social_graph_index].first_name[met], local_social_graph[social_graph_index].family_name[met], name);
        break;
    }
    case ENTITY_BEING_GROUP:
        (void)SHOW_ERROR("Unimplemented being group entity type");
        break;
    case ENTITY_OBJECT:
        (void)SHOW_ERROR("Unimplemented object entity type");
        break;
    case ENTITY_TERRITORY:
        (void)SHOW_ERROR("Unimplemented territory entity type");
        break;
    default:
        (void)SHOW_ERROR("Unimplemented entity type");
        break;
    }
}

/**
 * @brief Align learned preferences with another ape, depending upon
 * whether it is part of the ingroup or outgroup.
 * @param local_sim Pointer to the simulation
 * @param meeter_being Pointer to the ape doing the meeting
 * @param met_being Pointer to the ape being met
 * @param social_graph_index Array index within the meeter social graph for the met ape
 */
static void social_group_align_preferences(
    noble_simulation * local_sim,
    noble_being * meeter_being,
    noble_being * met_being,
    n_int social_graph_index)
{
    n_int i, incr = -1;
    noble_social * social_graph;

    /** don't align with yourself */
    if ((meeter_being==met_being) || (social_graph_index < 1)) return;

    /** get the social graph */
    social_graph = being_social(meeter_being);

    if (social_graph == 0L) return;

    /** the entry in the social graph for the other being shouldn't be empty */
    if (SOCIAL_GRAPH_ENTRY_EMPTY(social_graph,social_graph_index)) return;

    /** if you are friendly then make your preferences more similar,
    otherwise make them more disimilar */
    if (social_graph[social_graph_index].friend_foe >=
            (n_byte)social_respect_mean(local_sim,meeter_being))
    {
        incr = 1;
    }

    /** align preferences */
    for (i = 0; i < PREFERENCES; i++)
    {
        n_int resultant = meeter_being->learned_preference[i];
        if (resultant < met_being->learned_preference[i])
        {
            if ((incr > 0) || ((incr < 0) && (resultant > 0)))
            {
                resultant += incr;
            }
        }
        else if (resultant > met_being->learned_preference[i])
        {
            if ((incr > 0) || ((incr < 0) && (meeter_being->learned_preference[i]<255)))
            {
                resultant -= incr;
            }
        }
        meeter_being->learned_preference[i] = (n_byte)resultant;
    }
}

/**
 * @brief What is the pigmentation attractiveness of met ape to the meeter being ?
 * @param meeter_being Pointer to the ape doing the meeting
 * @param met_being Pointer to the ape being met
 * @return Attractiveness value
 */
static n_int social_attraction_pigmentation(
    noble_being * being_meeter,
    noble_being * being_met)
{
    n_int ppref, pdiff;
    n_byte fem = (FIND_SEX(GET_I(being_meeter)) == SEX_FEMALE);

    /** Either no preference for pigmentation, or only
        favour attractive mates */
    ppref = NATURE_NURTURE(
                GENE_PIGMENTATION_PREFERENCE(being_genetics(being_meeter)),
                being_meeter->learned_preference[PREFERENCE_MATE_PIGMENTATION_MALE+fem]);

    pdiff = GENE_PIGMENTATION(being_genetics(being_met)) - ppref;

    if ((pdiff >= -2) && (pdiff <= 2))
    {
        pdiff = ABS(pdiff);
        return (3 - pdiff);
    }
    return 0;
}

/**
 * What is the hair attractiveness of the met ape to the meeter being ?
 * @param meeter_being Pointer to the ape doing the meeting
 * @param met_being Pointer to the ape being met
 * @return Attractiveness value
 */
static n_int social_attraction_hair(
    noble_being * meeter_being,
    noble_being * met_being)
{
    n_int ppref, pdiff;
    n_byte fem = (FIND_SEX(GET_I(meeter_being)) == SEX_FEMALE);

    /** either no preference for hair length, or only
       favour attractive mates */

    ppref = NATURE_NURTURE(
                GENE_HAIR_PREFERENCE(being_genetics(meeter_being)),
                meeter_being->learned_preference[PREFERENCE_MATE_HAIR_MALE+fem]);
    pdiff = GENE_HAIR(being_genetics(met_being)) - ppref;

    if ((pdiff >= -2) && (pdiff <= 2))
    {
        pdiff = ABS(pdiff);
        return (3 - pdiff);
    }
    return 0;
}

/**
 * What is the height attractiveness of the met ape to the meeter being ?
 * @param meeter_being Pointer to the ape doing the meeting
 * @param met_being Pointer to the ape being met
 * @return Attractiveness value
 */
static n_int social_attraction_height(
    noble_being * meeter_being,
    noble_being * met_being)
{
    n_int ppref;
    n_byte fem = (FIND_SEX(GET_I(meeter_being)) == SEX_FEMALE);

    /** Either don't care about height or
       favour mates who are taller or shorter */

    ppref = NATURE_NURTURE(
                GENE_HEIGHT_PREFERENCE(being_genetics(meeter_being)),
                meeter_being->learned_preference[PREFERENCE_MATE_HEIGHT_MALE+fem]);

    /** prefer taller or shorter,
    < 8  don't care about height
    12-15 prefer taller
    8-11 prefer shorter*/
    if (ppref >= 8)
    {
        if ((ppref>=12) && (GET_H(met_being) > GET_H(meeter_being)))
        {
            /** prefer taller */
            return 1;
        }
        else
        {
            if ((ppref<12) && (GET_H(met_being) < GET_H(meeter_being)))
            {
                /** prefer shorter */
                return 1;
            }
        }
    }
    return 0;
}

/**
 * @brief What is the frame attractiveness of the met ape to the meeter being ?
 * @param meeter_being Pointer to the ape doing the meeting
 * @param met_being Pointer to the ape being met
 * @return Attractiveness value
 */
static n_int social_attraction_frame(
    noble_being * meeter_being,
    noble_being * met_being)
{
    n_int ppref;
    n_byte fem = (FIND_SEX(GET_I(meeter_being)) == SEX_FEMALE);

    /** Either don't care about frame or
    favour mates who are fatter or thinner */

    ppref = NATURE_NURTURE(
                GENE_FRAME_PREFERENCE(being_genetics(meeter_being)),
                meeter_being->learned_preference[PREFERENCE_MATE_FRAME_MALE+fem]);

    if ((ppref>6) && (ppref<=11) && (GET_BODY_FAT(met_being) > GET_BODY_FAT(meeter_being)))
    {
        /** prefer fatter */
        return 1;
    }
    else
    {
        if ((ppref>11) && (GET_BODY_FAT(met_being) < GET_BODY_FAT(meeter_being)))
        {
            /** prefer thinner */
            return 1;
        }
    }
    return 0;
}

/**
 * @brief What is the pheromone attractiveness of the met ape to the meeter being ?
 * Pheromone attraction is inversely proportional to genetic similarity.
 * @param meeter_being Pointer to the ape doing the meeting
 * @param met_being Pointer to the ape being met
 * @return Attractiveness value
 */
static n_int social_attraction_pheromone(
    noble_being * meeter_being,
    noble_being * met_being)
{
    n_int ch, i, different = 0;
    n_genetics * meeter_genetics = being_genetics(meeter_being);
    n_genetics * met_genetics = being_genetics(met_being);

    for (ch = 0; ch < CHROMOSOMES; ch++)
    {
        for (i = 0; i < 32; i++)
        {
            if (((meeter_genetics[ch] >> i) & 1) != ((met_genetics[ch] >> i) & 1))
            {
                different++;
            }
        }
    }
    if (different < MINIMUM_GENETIC_VARIATION)
    {
        return 0-GENE_INCEST_AVERSION(meeter_genetics);
    }
    else
    {
        return 1;
    }
}

/**
 * @brief If two beings have previously met return the social graph index
 * @param meeter_being Pointer to the ape doing the meeting
 * @param met_being Pointer to the ape being met
 * @param sim Pointer to the simulation
 * @return Array index within the social graph of the meeter, -1 if not found
 */
n_int get_noble_social(
    noble_being * meeter_being,
    noble_being * met_being,
    noble_simulation * sim)
{
    n_byte2 i;
    noble_social * graph = being_social(meeter_being);

    if (!graph) return -1;

    for (i=1; i<SOCIAL_SIZE_BEINGS; i++)
    {
        if (!SOCIAL_GRAPH_ENTRY_EMPTY(graph,i))
        {
            if (graph[i].entity_type==ENTITY_BEING)
            {
                if (being_name_comparison(met_being, graph[i].first_name[BEING_MET], graph[i].family_name[BEING_MET]))
                {
                    return i;
                }
            }
        }
    }
    return -1;
}

/**
 * @brief Returns the social graph index for the least familiar ape
 * @param meeter_being Pointer to the ape doing the meeting
 * @param met_being Pointer to the ape being met
 * @param sim Pointer to the simulation
 * @return Array index for the least familiar ape within the social graph of the meeter, -1 if not met
 */
static n_int get_stranger_link(
    noble_being * meeter_being,
    noble_being * met_being,
    noble_simulation * sim)
{
    n_byte2 i=1;
    n_int stranger_index=-1;
    n_byte2 stranger=65535, familiarity=0;
    n_int time_since_met;
    noble_social * graph = being_social(meeter_being);
    if (!graph) return 0;

    for (i=1; i<SOCIAL_SIZE_BEINGS; i++)
    {
        if (!SOCIAL_GRAPH_ENTRY_EMPTY(graph,i))
        {
            /** not a family relationship */
            if (!IS_FAMILY_MEMBER(graph,i))
            {
                /** minimum familiarity */
                familiarity = graph[i].familiarity;
                if (familiarity < stranger)
                {
                    /** Forget old stuff in order to avoid
                    	too much inflexibility */
                    time_since_met = sim->land->date - graph[i].date;
                    
                    if ((time_since_met >= SOCIAL_FORGET_DAYS) ||
                        (graph[i].date==0))
                    {
                        stranger = familiarity;
                        stranger_index = i;
                    }
                }
            }
        }
        else
        {
            /** If this entry is empty */
            stranger_index = i;
            break;
        }
    }
    return stranger_index;
}

/**
 * @brief When two beings meet this updates the social graph
 * @param meeter_being Pointer to the being doing the meeting
 * @param met_being Pointer to the being who was met
 * @param sim Pointer to the simulation
 * @return Array index within the social graph of the meeter, -1 if not met
 */
static n_int social_meet(
    noble_being * meeter_being,
    noble_being * met_being,
    noble_simulation * sim,
    n_byte location_type)
{
    n_int friend_or_foe, index = -1, stereotype_index = -1;

    n_byte2 familiarity = 0;
    noble_social * graph = being_social(meeter_being);
    n_byte2 met = 0;

    if (!graph) return -1;

    /** transmit any pathogens between the meeting beings */
    being_immune_transmit(meeter_being, met_being, PATHOGEN_TRANSMISSION_AIR);

    /** get the social graph index which will be used to represent this relation */
    index = get_noble_social(meeter_being,met_being,sim);
    if (index > 0)
    {
        familiarity = graph[index].familiarity;
        met=1;
    }
    else
    {
        /** get the index of an existing social graph entry
        which can be overwritten */
        index = get_stranger_link(meeter_being,met_being,sim);
    }

    if ((met == 1) || ((met == 0) && (index > 0)))
    {
#ifdef FEATURE_SET
        /** record the observable features of the being which was met */
        social_meet_update_features(
            meeter_being, met_being, index);

        /** get the social graph index of the corresponding stereotype */
        stereotype_index = social_get_stereotype(
            meeter_being, index);
#endif
        /** set the focus of attention to this being */
        GET_A(meeter_being,ATTENTION_ACTOR) = (n_byte)index;

        /** if we havn't met previously */
        if (met == 0)
        {
            if (stereotype_index > -1)
            {
                /** get the predisposition towards this stereotype */
                friend_or_foe =
                    graph[stereotype_index].friend_foe;
            }
            else
            {
                /** the prejudice function */
                friend_or_foe =
                    SOCIAL_RESPECT_NORMAL -
                    social_attraction_pheromone(meeter_being,met_being) +
                    social_attraction_pigmentation(meeter_being,met_being) +
                    social_attraction_height(meeter_being,met_being) +
                    social_attraction_frame(meeter_being,met_being) +
                    social_attraction_hair(meeter_being,met_being)
#ifdef EPISODIC_ON
                    + episodic_met_being_celebrity(sim,meeter_being,met_being)
#endif
                    ;

                /** TODO create new stereotype based upon the met being features */

            }
            /** Met another being */
            graph[index].entity_type = ENTITY_BEING;

            /** who did the meeting */
            graph[index].first_name[BEING_MEETER] = being_gender_name(meeter_being);
            graph[index].family_name[BEING_MEETER] = being_family_name(meeter_being);
            /** who was met */
            graph[index].first_name[BEING_MET] = being_gender_name(met_being);
            graph[index].family_name[BEING_MET] = being_family_name(met_being);

            /** initially no attraction */
            graph[index].attraction = 0;

            /** limit fof within range */
            if (friend_or_foe < 0) friend_or_foe = 0;
            if (friend_or_foe > 255) friend_or_foe = 255;
            graph[index].friend_foe = (n_byte)friend_or_foe;
#ifdef BRAINCODE_ON
            /** initialise the braincode associated with this individual */
            being_init_braincode(meeter_being,met_being,
                                 graph[index].friend_foe,BRAINCODE_EXTERNAL);
#endif
        }

        if (location_type == LOCATION_KNOWN)
        {
            /** this being was seen somewhere in my vicinity */
            graph[index].location[0] = (n_byte2)being_location_x(meeter_being);
            graph[index].location[1] = (n_byte2)being_location_y(meeter_being);
        }
        else {
            /** location unknown */
            graph[index].location[0] = 0;
            graph[index].location[1] = 0;
        }

        /** record the state of the met beting */
        graph[index].belief = being_state(met_being);

        /** date of the meeting */
        graph[index].date = sim->land->date;

        /** getting more familiar */
        if (familiarity < 65535)
        {
            graph[index].familiarity = familiarity + 1;
        }

        /** friendliness can be increased simply through familiarity */
        if (graph[index].friend_foe < 255)
        {
            graph[index].friend_foe++;
        }
    }
    return index;
}

/**
 * @brief Returns the social graph index of the given relationship type.
 * This can be used for example to search for the father of an
 * ape within the social graph.
 * @param meeter_being Pointer to the ape
 * @param relationship The type of relationship to search for
 * @param sim Pointer to the simulation
 * @return Array index of the social graph for the given type of relationship, -1 if not found
 */
n_int social_get_relationship(
    noble_being * meeter_being,
    n_byte relationship,
    noble_simulation * sim)
{
    n_int index;
    noble_social * meeter_social_graph;

    /** get the social graph */
    meeter_social_graph = being_social(meeter_being);

    if (meeter_social_graph == 0L)
    {
        return -1;
    }

    /** Search the social graph */
    for (index = 1; index < SOCIAL_SIZE_BEINGS; index++)
    {
        /** Is this the desired relationship type? */
        if (meeter_social_graph[index].relationship == relationship)
        {
            return index;
        }
    }
    return -1;
}

/**
 * @brief Set the relationship type between two apes
 * @param meeter_being Pointer to the ape doing the meeting
 * @param relationship The type of relationship
 * @param met_being Pointer to the ape being met
 * @param sim Pointer to the simulation
 * @return Array index of the meeter social graph, -1 if not met
 */
n_int social_set_relationship(noble_being * meeter_being,
                              n_byte relationship,
                              noble_being * met_being,
                              noble_simulation * sim)
{
    n_int index;
    noble_social * meeter_social_graph;

    /** no relationship specified */
    if (relationship == 0) return -1;

    /** create the social graph entry if necessary and return its array index */
    index = social_meet(meeter_being,met_being,sim, LOCATION_UNKNOWN);
    if (index > -1)
    {
        /** get the social graph */
        meeter_social_graph = being_social(meeter_being);

        if (meeter_social_graph == 0L)
        {
            return -1;
        }

        /** set the relationship type */
        meeter_social_graph[index].relationship = relationship;
    }
    return index;
}

/**
 * @brief When two apes meet within a given maximum range
 * this updates the social graph of both.
 * @param meeter_being Pointer to the ape doing the meeting
 * @param met_being Pointer to the ape being met
 * @param distance Distance between the apes
 * @param sim Pointer to the simulation
 * @return Array index of the meeter social graph, -1 if not met
 */
n_int social_network(
    noble_being * meeter_being,
    noble_being * met_being,
    n_int distance,
    noble_simulation *sim)
{
    n_int being_index = -1;
    if (distance < SOCIAL_RANGE)
    {
        being_index = social_meet(meeter_being, met_being, sim, LOCATION_KNOWN);
    }
    return being_index;
}

/**
 * @brief Grooming behavior
 * @param meeter_being Pointer to the ape doing the meeting
 * @param met_being Pointer to the ape being met
 * @param distance Distance between the apes
 * @param awake Whether the met being is awake
 * @param familiarity Familiarity with the met ape
 * @param sim Pointer to the simulation
 * @return 1 if grooming, 0 otherwise
 */
n_byte social_groom(
    noble_being * meeter_being,
    noble_being * met_being,
    n_int distance,
    n_int awake,
    n_byte2 familiarity,
    noble_simulation * sim)
{
    n_int meeter_index, met_index;
    n_byte max = 0, grooming = 0, groom_decisions, groomloc, fem;
    n_int gpref;
    n_int paraprob;

    /** hairy beings can carry more parasites */
    max = (n_byte)MAX_PARASITES(meeter_being);
    /** acquire parasites from the environment with some low probability,
    and existing parasites multiply */
    if (meeter_being->parasites < max)
    {
        paraprob = being_random(meeter_being);
        if (paraprob < PARASITE_ENVIRONMENT +
                (PARASITE_BREED*meeter_being->parasites))
        {
            meeter_being->parasites++;
        }
    }

    /** parasites sap energy */
    being_energy_delta(meeter_being, 0 - (PARASITE_ENERGY_COST*meeter_being->parasites));

    if (distance < PARASITE_HOP_MAX_DISTANCE)
    {
        /** hairy beings can carry more parasites */
        max = (n_byte)MAX_PARASITES(met_being);
        /** parasite transmission - e.g. flea hop */
        if ((met_being->parasites < max) &&
                (met_being->parasites < meeter_being->parasites))
        {
            met_being->parasites++;
            meeter_being->parasites--;
        }
    }

    /** social grooming removes parasites and alters
       social status relationships */
    if ((awake != FULLY_ASLEEP) &&
            (distance < GROOMING_MAX_SEPARATION) &&
            (being_speed(meeter_being) < MAX_SPEED_WHILST_GROOMING))
    {
        n_int  groomprob = being_random(meeter_being) & 16383;
        if (familiarity > 16) familiarity=16;

        /** is the groomee female? */
        fem = (FIND_SEX(GET_I(met_being)) == SEX_FEMALE);

        /** grooming preference */
        gpref = NATURE_NURTURE(
                    GENE_GROOM(being_genetics(meeter_being)),
                    meeter_being->learned_preference[PREFERENCE_GROOM_MALE+fem]);

        /** individuals which are familiar tend to groom more often */
        if (groomprob <
                GROOMING_PROB + (gpref*(1+familiarity)*GROOMING_PROB_HONOR*(1+met_being->honor)))
        {
            /** transmit pathogens via touch */
            being_immune_transmit(meeter_being, met_being, PATHOGEN_TRANSMISSION_TOUCH);
            being_immune_transmit(met_being, meeter_being, PATHOGEN_TRANSMISSION_TOUCH);

            /** pick a body location to groom */
            groomloc = GET_A(meeter_being,ATTENTION_BODY);
            groom_decisions = 0;
            while ((met_being->inventory[groomloc] & INVENTORY_GROOMED) && (groom_decisions<4))
            {
                met_being->inventory[groomloc] |= INVENTORY_GROOMED;
                groomloc = (n_byte)(being_random(meeter_being) % INVENTORY_SIZE);
                groom_decisions++;
            }
            /** groomed wounds disappear */
            if (met_being->inventory[groomloc] & INVENTORY_WOUND)
            {
                met_being->inventory[groomloc] = INVENTORY_GROOMED;
            }
            /** grooming location becomes the new focus of attention */
            GET_A(meeter_being, ATTENTION_BODY) = groomloc;

            episodic_interaction(sim, meeter_being, met_being, EVENT_GROOM, AFFECT_GROOM, groomloc);
            episodic_interaction(sim, met_being, meeter_being, EVENT_GROOMED, AFFECT_GROOM, groomloc);

            /** the two beings meet and become more friendly */
            meeter_index = social_meet(meeter_being, met_being, sim, LOCATION_KNOWN);
            if (meeter_index > -1)
            {
                met_index = social_meet(met_being, meeter_being, sim, LOCATION_KNOWN);
                if (met_index > -1)
                {
                    noble_social * graph = being_social(meeter_being);
                    if (!graph) return 0;

                    if ((graph[meeter_index].friend_foe)<255)
                    {
                        graph[meeter_index].friend_foe++;
                    }
                    if ((graph[met_index].friend_foe)<255)
                    {
                        graph[met_index].friend_foe++;
                    }
                }
            }
            /** Alter social status relations.
            The groomer gains status, since they are providing a service */
            being_honor_inc_dec(meeter_being, met_being);
            
            /** Decrement parasites */
            if (met_being->parasites >= PARASITES_REMOVED)
            {
                met_being->parasites-=PARASITES_REMOVED;
            }
            else
            {
                met_being->parasites = 0;
            }
            grooming = 1;
        }
    }

    return grooming;
}

/**
 * @brief Squabbling and fighting
 * @param meeter_being Pointer to the ape doing the meeting
 * @param met_being Pointer to the ape being met
 * @param distance Distance between the apes
 * @param is_female Whether the met being is female
 * @param sim Pointer to the simulation
 * @return The new being state: showforce/attack
 */
n_byte2 social_squabble(
    noble_being * meeter_being,
    noble_being * met_being,
    n_uint distance,
    n_int is_female,
    noble_simulation * sim)
{
    n_uint agro;
    n_byte2 ret_val = 0;
    noble_being * victor, * vanquished;
    n_int victor_index, vanquished_index;
    n_byte2 punchloc;
    n_vect2 delta;

    /** distance between beings */
    being_delta(met_being, meeter_being, &delta);

    /** battle with rival families */
    if ((being_family_first_name(meeter_being) != being_family_first_name(met_being)) &&
            (being_family_second_name(meeter_being) != being_family_second_name(met_being)))
    {
        being_facing_towards(meeter_being, &delta);
        /** high ranking apes will more aggressively defend their honor */
        agro = GENE_AGGRESSION(being_genetics(meeter_being));
        /** females are less agressive (less testosterone) */
        if (is_female) agro >>= 3;
        if (being_random(meeter_being) < agro*4096 + agro*meeter_being->honor*10)
        {
            /** who is the strongest ? */
            victor = meeter_being;
            vanquished = met_being;

            if (((being_random(meeter_being)&7)*being_energy(meeter_being)) <
                    ((being_random(meeter_being)&7)*being_energy(met_being)))
            {
                victor = met_being;
                vanquished = meeter_being;
            }

            vanquished_index = social_meet(victor, vanquished, sim, LOCATION_KNOWN);
            if (vanquished_index > -1)
            {
                victor_index = social_meet(vanquished, victor, sim, LOCATION_KNOWN);
                if (victor_index > -1)
                {
                    noble_social * victor_social_graph = being_social(victor);
                    noble_social * vanquished_social_graph = being_social(vanquished);

                    if ((!victor_social_graph) || (!vanquished_social_graph)) return 0;

                    /** victor disrespects the vanquished */
                    if (victor_social_graph[vanquished_index].friend_foe > SQUABBLE_DISRESPECT)
                    {
                        victor_social_graph[vanquished_index].friend_foe-=SQUABBLE_DISRESPECT;
                    }
                    /** vanquished disrespects the victor */
                    if (vanquished_social_graph[victor_index].friend_foe > SQUABBLE_DISRESPECT)
                    {
                        vanquished_social_graph[victor_index].friend_foe-=SQUABBLE_DISRESPECT;
                    }
                }
            }
            /** victor increases in honor */
            if (victor->honor < 255-SQUABBLE_HONOR_ADJUST) victor->honor += SQUABBLE_HONOR_ADJUST;
            /** vanquished decreases in honor */
            if (vanquished->honor > SQUABBLE_HONOR_ADJUST) vanquished->honor -= SQUABBLE_HONOR_ADJUST;

            punchloc = being_random(victor) % INVENTORY_SIZE;
            if (distance > SQUABBLE_SHOW_FORCE_DISTANCE)
            {
                /** show of force */
                vanquished->inventory[punchloc] = 0;
                being_energy_delta(victor, 0 - SQUABBLE_ENERGY_SHOWFORCE);
                being_energy_delta(vanquished, 0 -SQUABBLE_ENERGY_SHOWFORCE);

                ret_val |= BEING_STATE_SHOWFORCE;
            }
            else
            {
                /** attack */
                vanquished->inventory[punchloc] = INVENTORY_WOUND;
                being_energy_delta(victor, 0 - SQUABBLE_ENERGY_ATTACK);
                being_energy_delta(vanquished, 0 -SQUABBLE_ENERGY_ATTACK);
                being_honor_swap(victor, vanquished);
                ret_val |= BEING_STATE_ATTACK;
            }

            /** remember the fight */

            episodic_interaction(sim, victor, vanquished, EVENT_HIT, AFFECT_SQUABBLE_VICTOR, punchloc);
            episodic_interaction(sim, vanquished, victor, EVENT_HIT_BY, AFFECT_SQUABBLE_VANQUISHED, punchloc);

            /** vanquished turns away */
            if (meeter_being == vanquished)
            {
                n_vect2 negative_delta, zero = {0};

                vect2_subtract(&negative_delta, &zero, &delta);

                being_facing_towards(vanquished, &negative_delta);
            }
            else
            {
                being_facing_towards(vanquished, &delta);
            }

            /** vanquished flees */
            being_set_speed(vanquished, SQUABBLE_FLEE_SPEED);
        }
        return ret_val;
    }
    return 0;
}

/**
 * @brief Returns the average friend or foe value
 * @param sim Pointer to the simulation
 * @param local_being Pointer to the ape
 * @return The average friend or foe value for all social graph entries
 */
n_uint social_respect_mean(
    noble_simulation * sim,
    noble_being *local_being)
{
    n_uint noble_socials=0,average=0;
    noble_social * local_social_graph;
    n_int i;

    local_social_graph = being_social(local_being);
    if (!local_social_graph) return 0;

    /** note that this includes the self */
    for (i=0; i<SOCIAL_SIZE; i++)
    {
        if (!SOCIAL_GRAPH_ENTRY_EMPTY(local_social_graph,i))
        {
            noble_socials++;
            average += (n_uint)(local_social_graph[i].friend_foe);
        }
    }
    if (noble_socials>0)
    {
        return average/noble_socials;
    }
    return SOCIAL_RESPECT_NORMAL;
}

/**
 * @brief Update for a conception event. This stores the date,
 * details of the father and resets drives and goals.
 * @param female Pointer to the mother
 * @param male Pointer to the father
 * @param today The current date
 * @param sim Pointer to the simulation
 */
/*static*/ void social_conception(
    noble_being * female,
    noble_being * male,
    noble_simulation * sim)
{
    if ((male == 0L) || (female == 0L))
    {
        return;
    }
    
    body_genetics(sim->beings, sim->num, being_fetal_genetics(female), being_genetics(female), being_genetics(male), female->seed);

    /** store the date of conception */
    female->date_of_conception = sim->land->date;
    
    female->father_name[0]   = being_gender_name(male);
    female->father_name[1]   = being_first_name(male);

    if (male->generation_min < female->generation_min)
    {
        female->child_generation_min = male->generation_min;
    }
    else
    {
        female->child_generation_min = female->generation_min;
    }

    if (male->generation_max > female->generation_max)
    {
        female->child_generation_max = male->generation_max;
    }
    else
    {
        female->child_generation_max = female->generation_max;
    }

    /** reset sex drive and goal */
    being_reset_drive(female, DRIVE_SEX);
    being_reset_drive(male, DRIVE_SEX);

    female->goal[0]=GOAL_NONE;
    male->goal[0]=GOAL_NONE;

    /** remember the event */
    episodic_interaction(sim, female, male, EVENT_MATE,  (GENE_MATE_BOND(being_genetics(female))*AFFECT_MATE), 0);
    episodic_interaction(sim, male, female, EVENT_MATE,  (GENE_MATE_BOND(being_genetics(male))*AFFECT_MATE), 0);
}

/**
 * @brief Mating behavior
 * @param meeter_being Pointer to the ape doing the meeting
 * @param met_being Pointer to the ape whi is being met
 * @param today The current date
 * @param being_index Array index for the met individual within the social graph of the meeter
 * @param distance The Distance between the two apes
 * @param sim Pointer to the simulation
 * @return The being state: reproducing or not
 */
n_int social_mate(
    noble_being * meeter_being,
    noble_being * met_being,
    n_int being_index,
    n_int distance,
    noble_simulation * sim)
{
    n_int loc_state = 0;
    n_int attraction = 0;
    n_int attract;
/*    n_byte2 matingprob;*/
    noble_social * meeter_social_graph = being_social(meeter_being);

    if (!meeter_social_graph) return -1;

    /*if ((being_drive(meeter_being, DRIVE_SEX) > THRESHOLD_SEEK_MATE) &&
            (being_drive(met_being, DRIVE_SEX) > THRESHOLD_SEEK_MATE))*/
    {
        
        /** mating is probabilistic, with a bias towards
            higher status individuals */
        
        /*
        
        matingprob = being_random(meeter_being);
        if (matingprob <
                (32000 + (n_byte2)(met_being->honor)*
                 GENE_STATUS_PREFERENCE(being_genetics(meeter_being))*MATING_PROB)) */
        {
            
            
            /** attractiveness based upon various criteria */
            attraction = 1 +
                         social_attraction_pheromone(meeter_being,met_being) +
                         social_attraction_pigmentation(meeter_being,met_being) +
                         social_attraction_height(meeter_being,met_being) +
                         social_attraction_frame(meeter_being,met_being) +
                         social_attraction_hair(meeter_being,met_being)
#ifdef EPISODIC_ON
                         + episodic_met_being_celebrity(sim,meeter_being,met_being)
#endif
                         ;

            /** some minimum level of attraction required for pair bonding */
            /*if (meeter_social_graph[being_index].attraction > PAIR_BOND_THRESHOLD)
            {
                attraction++;
                */
                if (distance < MATING_RANGE)
                {
                    /** transmit pathogens */
                    being_immune_transmit(meeter_being, met_being, PATHOGEN_TRANSMISSION_SEX);
                    being_immune_transmit(met_being, meeter_being, PATHOGEN_TRANSMISSION_SEX);
                    /** check opposite sexes */
                    if ((FIND_SEX(GET_I(meeter_being)) == SEX_FEMALE) &&
                            (FIND_SEX(GET_I(met_being)) != SEX_FEMALE))
                    {
                        if (being_pregnant(meeter_being) == 0)
                        {
                            social_conception(meeter_being, met_being, sim);
                        }
                    }
                }
            /*
            }
            else
            {
                attraction--;
            }*/
        }

        attract = meeter_social_graph[being_index].attraction;
        if (attraction > 0)
        {
            if (attraction < PAIR_BOND_THRESHOLD*4)
            {
                if (attract < 255-attraction) attract+=attraction;
            }
        }
        else
        {
            if (attract > -attraction)
            {
                attract += attraction;
            }
            else
            {
                attract=0;
            }
        }
        meeter_social_graph[being_index].attraction=(n_byte)attract; /**< '=' : conversion from 'n_int' to 'n_byte', possible loss of data */
    }
    return loc_state;
}

/**
 * @brief When two beings meet agree on the name for the current territory
 * @param meeter_being Pointer to the being doing the meeting
 * @param met_being Pointer to the being whi is being met
 * @param being_index Array index for the met individual within the social graph of the meeter
 * @param meeter_graph Pointer to the social graph of the meeter
 * @param respect_mean Average friend of foe value within the social graph
 */
static void social_chat_territory(
    noble_being * meeter_being,
    noble_being * met_being,
    n_int being_index,
    noble_social * meeter_graph,
    n_uint respect_mean)
{
#ifdef TERRITORY_ON
    n_int idx=0,idx2,i=0,x,y;

    idx = APESPACE_TO_TERRITORY(being_location_y(meeter_being))*TERRITORY_DIMENSION +
          APESPACE_TO_TERRITORY(being_location_x(meeter_being));
    if (meeter_being->territory[idx].name==0)
    {
        i=0;
        for (y=-1; y<=1; y++)
        {
            for (x=-1; x<=1; x++)
            {
                if (!((x==0)&&(y==0)))
                {
                    idx2 = idx + (y*TERRITORY_DIMENSION+x);
                    if (idx2<0) idx2+=TERRITORY_AREA;
                    if (idx2>=TERRITORY_AREA) idx2-=TERRITORY_AREA;
                    i = meeter_being->territory[idx2].name;
                    if (i>0)
                    {
                        y = 2;
                        break;
                    }
                }
            }
        }
        /** give the current place a name at random */
        if (i == 0)
        {
            i = 1 + (n_byte)(being_random(meeter_being) & 255);
        }
        meeter_being->territory[idx].name = (n_byte)i;
    }

    /** take advice from more honorable friends */
    if (meeter_graph[being_index].friend_foe >= respect_mean)
    {
        if (being_honor_compare(met_being, meeter_being) == 1)
        {
            if (met_being->territory[idx].name > 0)
            {
                meeter_being->territory[idx].name =
                    met_being->territory[idx].name;
            }
        }
        else
        {
            if ((being_honor_compare(met_being, meeter_being) == -1) &&
                    (meeter_being->territory[idx].name > 0))
            {
                met_being->territory[idx].name =
                    meeter_being->territory[idx].name;
            }
        }
    }
#endif
}

/**
 * @brief Dialogue between beings
 * @param meeter_being Pointer to the being doing the meeting
 * @param met_being Pointer to the being which is being met
 * @param being_index Array index for the met individual within the social graph of the meeter
 * @param sim Pointer to the simulation
 * @return A non-zero value if speaking
 */
n_int social_chat(
    noble_being * meeter_being,
    noble_being * met_being,
    n_int being_index,
    noble_simulation * sim)
{
    n_int idx,i=0;
    n_byte relationship_index;
    n_byte2 name, family;
    n_int replace;
    n_int speaking = 0;
    noble_social * meeter_graph = being_social(meeter_being);
    noble_social * met_graph = being_social(met_being);
    n_uint respect_mean = social_respect_mean(sim,meeter_being);

    if (!meeter_graph) return 0;

    if (!met_graph) return 0;

    /** agree upon terrirory */
    social_chat_territory(meeter_being, met_being,being_index,meeter_graph,respect_mean);

    /** do I respect their views ? */
    if ((meeter_graph[being_index].friend_foe) >= respect_mean)
    {
        episodic_interaction(sim, meeter_being, met_being, EVENT_CHAT, AFFECT_CHAT, 0);
        /** pick one of the individuals from their graph */
        idx=-1;
        if (meeter_being->goal[0]==GOAL_MATE)
        {
            /** ask about an individual we're searching for */
            for (i=1; i<SOCIAL_SIZE_BEINGS; i++)
            {
                if (!SOCIAL_GRAPH_ENTRY_EMPTY(met_graph,i))
                {
                    if ((met_graph[i].first_name[BEING_MET]==meeter_being->goal[1]) &&
                            (met_graph[i].family_name[BEING_MET]==meeter_being->goal[2]))
                    {
                        idx=i;
                        break;
                    }
                }
            }
        }
        if (idx == -1)
        {
            /** what type of family relationship is currently being attended to */
            relationship_index = GET_A(meeter_being,ATTENTION_RELATIONSHIP);
            if (relationship_index>0)
            {
                idx = social_get_relationship(meeter_being,relationship_index,sim);
            }
            else
            {
                /** choose randomly */
                idx = 1+(being_random(meeter_being)%(SOCIAL_SIZE_BEINGS-1));
            }
        }

        if (idx > -1)
        {
            /** have I already met this individual? */
            name = met_graph[idx].first_name[BEING_MET];
            family = met_graph[idx].family_name[BEING_MET];
            if (!((name==0) && (family==0)))
            {
                for (i=1; i<SOCIAL_SIZE_BEINGS; i++)
                {
                    if (!SOCIAL_GRAPH_ENTRY_EMPTY(meeter_graph,i))
                    {
                        if ((meeter_graph[i].first_name[BEING_MET]==name) &&
                                (meeter_graph[i].family_name[BEING_MET]==family))
                        {
                            break;
                        }
                    }
                }

                if (i<SOCIAL_SIZE_BEINGS)
                {
                    /** was already met */
                    if (being_honor_compare(met_being, meeter_being) == 1)
                    {
                        meeter_graph[i].friend_foe++;
                    }
                    if (being_honor_compare(met_being, meeter_being) == -1)
                    {
                        meeter_graph[i].friend_foe--;
                    }
                    if (meeter_graph[i].familiarity < 65535) meeter_graph[i].familiarity++;

                    /** update this being's belief */
                    if (met_graph[idx].date > meeter_graph[i].date)
                    {
                        /** belief about location */
                        meeter_graph[i].location[0] = met_graph[idx].location[0];
                        meeter_graph[i].location[1] = met_graph[idx].location[1];
                        /** belief about state */
                        meeter_graph[i].belief = met_graph[idx].belief;
                        meeter_graph[i].date = met_graph[idx].date;
                    }
                    speaking |= BEING_STATE_SPEAKING;
                }
                else
                {
                    /** if we have never met then add to my graph as someone I've
                    "heard of". This is like a prior expectation or second
                    hand information.
                    The least familiar relationship is replaced */
                    replace = get_stranger_link(meeter_being,met_being,sim);
                    if (replace > -1)
                    {
                        io_copy((n_byte *)&met_graph[idx], (n_byte *)&meeter_graph[replace], sizeof(noble_social));
                        meeter_graph[replace].attraction = 0;
                        speaking |= BEING_STATE_SPEAKING;

                        /** if this is a family member of the previously unknown
                            being then make sure that the family member type is
                            set to OTHER - i.e. not my family someone else's */
                        if (IS_FAMILY_MEMBER(met_graph,idx))
                        {
                            meeter_graph[replace].relationship =
                                meeter_graph[replace].relationship+(OTHER_MOTHER-RELATIONSHIP_MOTHER);
                        }
#ifdef BRAINCODE_ON
                        /** initialise the braincode */
                        being_init_braincode(
                            meeter_being,met_being,
                            met_graph[idx].friend_foe,
                            BRAINCODE_EXTERNAL);
#endif
                    }
                }
            }
        }
    }
    being_reset_drive(met_being, DRIVE_SOCIAL);
    being_reset_drive(meeter_being, DRIVE_SOCIAL);

#ifdef BRAINCODE_ON
    brain_dialogue(
        sim,1,meeter_being,met_being,
        being_braincode_external(meeter_being),
        being_braincode_external(met_being),
        being_index);

#endif
#ifdef EPISODIC_ON
    social_group_align_preferences(
        sim,meeter_being,met_being,being_index);
#endif
    if (speaking != 0)
    {
        being_add_state(meeter_being, BEING_STATE_SPEAKING);
        being_add_state(met_being, BEING_STATE_SPEAKING);
    }
    return speaking;
}

/**
 * @brief Goal oriented behavior
 * @param local Pointer to the ape
 * @param loc_f The direction facing
 * @return The new direction facing
 */
void social_goals(
    noble_being * local)
{
    n_int delta_x=0, delta_y=0, distsqr;
    n_byte2 goal;
    n_vect2 delta_vector,location_vector;
    goal = local->goal[0];
    switch(goal)
    {
        /** move towards a location */
    case GOAL_LOCATION:
    {
        if ((being_state(local) & BEING_STATE_SWIMMING) == 0)
        {
            vect2_byte2(&delta_vector, (n_byte2 *)&(local->goal[1]));
            vect2_byte2(&location_vector, being_location(local));
            vect2_subtract(&delta_vector, &location_vector, &delta_vector);
            being_facing_towards(local, &delta_vector);
        }
        break;
    }
    }

    /** are we there yet? */
    if (goal==GOAL_LOCATION)
    {
        distsqr = delta_x*delta_x + delta_y*delta_y;
        if ((distsqr < GOAL_RADIUS) ||
                ((being_state(local) & BEING_STATE_SWIMMING) != 0))
        {
            /** destination reached - goal cancelled */
            local->goal[0] = GOAL_NONE;
            /** clear any script override */
            local->script_overrides -= OVERRIDE_GOAL;
        }
    }

    /** decrement the goal counter */
    if (local->goal[3] > 0)
    {
        local->goal[3]--;
    }
    else
    {
        /** timed out */
        local->goal[0] = GOAL_NONE;
    }
}

void social_initial_loop(noble_simulation * local, noble_being * local_being, void * data)
{
    n_uint respect_mean = social_respect_mean(local,local_being);
    n_uint social_loop = 0;
    n_vect2 location, sum_delta = {0,0};
    n_int   familiar_being_count = 0;
    vect2_byte2(&location,(n_byte2 *)&(local_being->social_x));
    while ( social_loop < SOCIAL_SIZE_BEINGS )
    {
        noble_social * specific_individual = &(being_social(local_being)[social_loop]);
        noble_being  * specific_being;

        if (!specific_individual) return;

        if (!SOCIAL_GRAPH_ENTRY_EMPTY(being_social(local_being),social_loop))
        {
            specific_being = being_find_name(local, specific_individual->first_name[BEING_MET], specific_individual->family_name[BEING_MET]);

            if (specific_being != 0L)
            {
                n_vect2 weighted_delta;
                n_vect2 familiar_location;
                n_int	local_friend_or_foe = specific_individual->friend_foe;
                n_int	distance_squared;

                local_friend_or_foe -= respect_mean;

                familiar_being_count++;

                vect2_byte2(&familiar_location,(n_byte2 *)&(specific_being->social_x));

                vect2_subtract(&weighted_delta, &familiar_location, &location);

                distance_squared = vect2_dot(&weighted_delta, &weighted_delta, 1, 512);

                if (distance_squared<0) distance_squared=0;  /**< Bug fix for division by zero on the following line */

                vect2_d(&sum_delta,&weighted_delta, local_friend_or_foe * 2048,
                        (distance_squared + 1));
            }
        }

        social_loop++;
    }

    if (familiar_being_count != 0)
    {
        vect2_d(&location,&sum_delta,1,(familiar_being_count*20));
    }
    vect2_back_byte2(&location,(n_byte2 *)&(local_being->social_nx));
}

/**
 * @brief
 * @param local Pointer to the simulation object
 * @param local_being Pointer to the being
 */
void social_secondary_loop(noble_simulation * local, noble_being * local_being, void * data)
{
    local_being->social_x = local_being->social_nx;
    local_being->social_y = local_being->social_ny;
}
