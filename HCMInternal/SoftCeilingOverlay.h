#pragma once
#include "pch.h"
#include "IOptionalCheat.h"
#include "GameState.h"
#include "DIContainer.h"

//TODO
/*
Soft ceilings only relavent to h3, od, hr, and h4


with h3 as a base:
data for soft ceilings is stored in both a tagblock in the scenario tag, and in multiple .sddt (structure design) tags
the former contains metadata, specifically:
	* bits for whether it applies to bipeds, vehicles, camera, and huge vehicles
	* the type of soft ceiling (Acceleration, slippy, soft kill)
	* the name (stringID)
the later contains the position data, specifically:
	a soft ceiling tagblock containing the name and type, and
	* the "soft ceiling triangles" tagblock, containing:
		* plane that makes up the tri
		* "bounding sphere center" and "bounding sphere radius" (first pass collision detection optimization? we may be able to ignore this)
		* pos3 of vertex 0, 1, and 2.

* things appear to be laid out exactly the same for the other games.
* which raises the question - how does the game associate the scnr metadata to the sddt data? via the name? (stringID?) ? 
	
More specifically, how would we implement GetSoftCeilingData ? 
GetScenarioAddress-> go to Soft Ceiling TagBlock, store that data in a metadata map by stringID key
Need a TagTableRange service, so we can grab all tags of sddt type
for each sddt go to the soft ceiling tagblock, find matching stringID in the metadata map. 
parse each triangle in the soft ceiling tris tagblock
pushback each tri into a vec of SoftCeilingData that contains:
	* the type
	* what it applies to
	* the 3 vertexes
*/





class SoftCeilingOverlayUntemplated { public: virtual ~SoftCeilingOverlayUntemplated() = default; };
class SoftCeilingOverlay : public IOptionalCheat
{
private:

	std::unique_ptr<SoftCeilingOverlayUntemplated> pimpl;

public:
	SoftCeilingOverlay(GameState gameImpl, IDIContainer& dicon);
	~SoftCeilingOverlay();

	std::string_view getName() override { return nameof(SoftCeilingOverlay); }

};