#pragma once
#ifndef BIONICS_H
#define BIONICS_H

#include "bodypart.h"
#include "string_id.h"

#include <set>
#include <string>
#include <vector>
#include <map>

class player;
class JsonObject;
class JsonIn;
class JsonOut;
struct quality;
using quality_id = string_id<quality>;
struct mutation_branch;
using trait_id = string_id<mutation_branch>;
struct bionic_data;
using bionic_id = string_id<bionic_data>;

struct bionic_data {
    bionic_data();

    std::string name;
    std::string description;
    /** Power cost on activation */
    int power_activate = 0;
    /** Power cost on deactivation */
    int power_deactivate = 0;
    /** Power cost over time, does nothing without a non-zero charge_time */
    int power_over_time = 0;
    /** How often a bionic draws power while active in turns */
    int charge_time = 0;
    /** Power bank size **/
    int capacity = 0;

    /** True if a bionic can be used by an NPC and installed on them */
    bool npc_usable = false;
    /** True if a bionic is a "faulty" bionic */
    bool faulty = false;
    bool power_source = false;
    /** Is true if a bionic is an active instead of a passive bionic */
    bool activated = false;
    /** If true, then the bionic only has a function when activated, else it causes
     *  it's effect every turn.
     */
    bool toggled = false;
    /**
     * If true, this bionic is a gun bionic and activating it will fire it.
     * Prevents all other activation effects.
     */
    bool gun_bionic = false;
    /**
     * If true, this bionic is a weapon bionic and activating it will
     * create (or destroy) bionic's fake_item in user's hands.
     * Prevents all other activation effects.
     */
    bool weapon_bionic = false;
    /**
     * If true, this bionic can provide power to powered armor.
     */
    bool armor_interface = false;
    /**
     * Body part slots used to install this bionic, mapped to the amount of space required.
     */
    std::map<body_part, size_t> occupied_bodyparts;
    /**
     * Fake item created for crafting with this bionic available.
     * Also the item used for gun bionics.
     */
    std::string fake_item;
    /**
     * Mutations/trait that are removed upon installing this CBM.
     * E.g. enhanced optic bionic may cancel HYPEROPIC trait.
     */
    std::vector<trait_id> canceled_mutations;
    /**
     * Additional bionics that are installed automatically when this
     * bionic is installed. This can be used to install several bionics
     * from one CBM item, which is useful as each of those can be
     * activated independently.
     */
    std::vector<bionic_id> included_bionics;

    /**
     * Id of another bionic which this bionic can upgrade.
     */
    bionic_id upgraded_bionic;
    /**
     * Upgrades available for this bionic (opposite to @ref upgraded_bionic).
     */
    std::set<bionic_id> available_upgrades;

    bool is_included( const bionic_id &id ) const;
};

struct bionic {
    bionic_id id;
    int         charge  = 0;
    char        invlet  = 'a';
    bool        powered = false;

    bionic()
        : id( "bio_batteries" ) { }
    bionic( bionic_id pid, char pinvlet )
        : id( std::move( pid ) ), invlet( pinvlet ) { }

    bionic_data const &info() const {
        return *id;
    }

    int get_quality( const quality_id &quality ) const;

    void serialize( JsonOut &json ) const;
    void deserialize( JsonIn &jsin );
};

// A simpler wrapper to allow forward declarations of it. std::vector can not
// be forward declared without a *definition* of bionic, but this wrapper can.
class bionic_collection : public std::vector<bionic>
{
};

void check_bionics();
void finalize_bionics();
void reset_bionics();
void load_bionic( JsonObject &jsobj ); // load a bionic from JSON
char get_free_invlet( player &p );
std::string list_occupied_bps( const bionic_id &bio_id, const std::string &intro,
                               const bool each_bp_on_new_line = true );

#endif
