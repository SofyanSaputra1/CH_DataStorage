#include <utility>
#include <vector>
#include <string>
#include <eosiolib/eosio.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/contract.hpp>
#include <eosiolib/crypto.h>

using eosio::key256;
using eosio::indexed_by;
using eosio::const_mem_fun;
using eosio::asset;
using eosio::permission_level;
using eosio::action;
using eosio::print;
using eosio::name;
using std::string;

class CHVotingContract : public eosio::contract {
  public:
    CHVotingContract(account_name self) : contract(self),
      researchforms(_self, _self),
      mpvotes(_self, _self),
      opteddatasets(_self, _self){}
	
    //@abi action
    void submitform(account_name researchid, string formdata, uint64_t acceptedpercentage, uint64_t count) {
      account_name owner = _self;

      require_auth(owner);

      // Store new research form
      researchforms.emplace(_self, [&](auto& researchform){
        researchform.researchid = researchid;
        researchform.formdata = formdata;
        researchform.acceptedpercentage = acceptedpercentage;
        researchform.count = count;
      });
    }

    //@abi action
    void vote(account_name mp, account_name researchid, uint64_t stakedcoins) {
      // Store new MP
      mpvotes.emplace(_self, [&](auto& mpvote){
          mpvote.mp = mp;
          mpvote.researchid = researchid;
          mpvote.stakedcoins = stakedcoins;
      });
    }

    //@abi action
    void votedeltas(account_name researchid) {
			auto iterator = mpvotes.find(researchid);
			uint64_t totalstake;
			uint64_t yaystake;
			for(auto it = iterator; it!=mpvotes.end(); it++)
			{
				if(it->stakedcoins>0)
				{
					yaystake+=it->stakedcoins;
				}
				totalstake+=it->stakedcoins;
			}
			uint64_t percentage=(uint64_t)((yaystake*100)/totalstake);
			auto researchform = researchforms.find(researchid);
			researchforms.modify(researchform, N(testaccount),[&]( auto& rform)
			{
				rform.acceptedpercentage = percentage;
			});
			if(percentage>=66)
			{
				//TODO add functionality here
			}
    }
	
		//@abi action
    void optin(account_name researchid,string optedhash) {
			opteddatasets.emplace(_self, [&](auto& opteddataset)
				{
							opteddataset.researchid = researchid;
							opteddataset.optedhash = optedhash;
						});
		}

  private:

    //@abi table researchform i64
    struct researchform {
        account_name researchid;

        string formdata;

        uint64_t acceptedpercentage;
		
		uint64_t count;

        uint64_t primary_key()const { return researchid; }

        EOSLIB_SERIALIZE( researchform, (researchid)(formdata)(acceptedpercentage)(count) )
    };

    typedef eosio::multi_index< N(researchform), researchform> research_form_index;

    //@abi table mpvotes i64
    struct mpvotes {
        account_name mp;
        account_name researchid;
		
		    uint64_t stakedcoins;

        uint64_t primary_key()const { return researchid; }
        uint64_t secondary_key()const { return mp; }

        EOSLIB_SERIALIZE( mpvotes, (mp)(researchid) )
    };

    typedef eosio::multi_index< N(mpvotes), mpvotes> mp_votes_index;

	
    //@abi table opteddata i64
    struct opteddata {
        account_name researchid;
        string optedhash;
        uint64_t primary_key()const { return researchid; }

        EOSLIB_SERIALIZE( opteddata, (researchid)(optedhash) )
    };

    typedef eosio::multi_index< N(opteddata), opteddata> opteddata_index;
	
    research_form_index researchforms;
    mp_votes_index mpvotes;
		opteddata_index opteddatasets;
};

EOSIO_ABI(CHVotingContract,(submitform) (vote) (votedeltas) (optin))