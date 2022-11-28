#define Ldap_DEPRECATED 1 //errors without this header
#include "header/Ldap.hpp"
#include "../share/protocol.h"
#include <ldap.h>
#include <lber.h>

namespace twServer{

    Ldap::Ldap(std::string username, std::string password){
        m_username = username;
        m_password = password;
    }

    Ldap::~Ldap(){}

    //printing the excat information found in the query
    void Ldap::printSearchResult(LDAP *ldapHandle, LDAPMessage *searchResult){
        LDAPMessage *searchResultEntry;

        //iterating through results to print DN
        for (searchResultEntry = ldap_first_entry(ldapHandle, searchResult);
            searchResultEntry != NULL;
            searchResultEntry = ldap_next_entry(ldapHandle, searchResultEntry)){

            std::cout << "DN: " << ldap_get_dn(ldapHandle, searchResultEntry) << "\n";

            BerElement *ber;

            //iterating through results to get exact information
            char *searchResultEntryAttribute;
            for (searchResultEntryAttribute = ldap_first_attribute(ldapHandle, searchResultEntry, &ber);
                searchResultEntryAttribute != NULL;
                searchResultEntryAttribute = ldap_next_attribute(ldapHandle, searchResultEntry, ber)){

                BerValue **vals;
                if ((vals = ldap_get_values_len(ldapHandle, searchResultEntry, searchResultEntryAttribute)) != NULL){
                    for (int i = 0; i < ldap_count_values_len(vals); i++){
                        std::cout << searchResultEntryAttribute << ": " << vals[i]->bv_val << "\n";
                    }
                    ldap_value_free_len(vals);
                }

                // free memory
                ldap_memfree(searchResultEntryAttribute);
            }
            // free memory
            if (ber != NULL){
                ber_free(ber, 0);
            }
        }
    }

    //performs search for user with given credentials and returns 1 upon finding a matching entry
    int Ldap::checkIfUserExists(){
        int userExists = 0;

        //setting up data for search
        //concrete user to look for
        std::string fullUserName = "uid=" + getUsername() + ",ou=people,dc=technikum-wien,dc=at";
        strcpy(ldapBindUser, fullUserName.c_str());
        strcpy(ldapBindPassword, getPassword().c_str());

        std::string filter = "(uid=" + getUsername() + "*)";
        setLdapSearchFilter(filter.c_str());
        const char *ldapSearchResultAttributes[] = {"uid", "cn", NULL};

        //return code
        int rc = 0; 

        //set up ldap connection
        LDAP *ldapHandle;
        rc = ldap_initialize(&ldapHandle, getLdapURL());
        if (rc != LDAP_SUCCESS){
            std::cerr << "Error: ldap_init failed\n";
            return -1;
        }
        std::cout << "LDAP server initialized\n";

        //set version options
        rc = ldap_set_option(ldapHandle, LDAP_OPT_PROTOCOL_VERSION, &ldapVersion);             
        if (rc != LDAP_OPT_SUCCESS){
            std::cerr << "Error: ldap_set_option(PROTOCOL_VERSION): " << ldap_err2string(rc) << "\n";
            ldap_unbind_ext_s(ldapHandle, NULL, NULL);
            return -1;
        }

        //start connection
        rc = ldap_start_tls_s(ldapHandle, NULL, NULL);
        if (rc != LDAP_SUCCESS){
            std::cerr << "ldap_start_tls_s(): " << ldap_err2string(rc) << "\n";
            ldap_unbind_ext_s(ldapHandle, NULL, NULL);
            return -1;
        }
        std::cout << "Connected to LDAP server\n";

        //bind credentials
        BerValue bindCredentials;
        bindCredentials.bv_val = (char *)ldapBindPassword;
        bindCredentials.bv_len = strlen(ldapBindPassword);
        BerValue *servercredp; // server's credentials

        rc = ldap_sasl_bind_s(ldapHandle, ldapBindUser, LDAP_SASL_SIMPLE, &bindCredentials, NULL, NULL, &servercredp);
        if (rc != LDAP_SUCCESS)
        {
            std::cerr << "LDAP bind error: " << ldap_err2string(rc) << "\n";
            ldap_unbind_ext_s(ldapHandle, NULL, NULL);
            return -1;
        }

        //perform search
        LDAPMessage *searchResult;
        rc = ldap_search_ext_s(
            ldapHandle,
            getLdapSearchBase(),
            getLdapSearchScope(),
            getLdapSearchFilter(),
            (char **)ldapSearchResultAttributes,
            0,
            NULL,
            NULL,
            NULL,
            500,
            &searchResult);
        if (rc != LDAP_SUCCESS)
        {
            std::cerr << "LDAP search error: " << ldap_err2string(rc) << "\n";
            ldap_unbind_ext_s(ldapHandle, NULL, NULL);
            return -1;
        }

        //to see found results, only for server and debugging
        Ldap::printSearchResult(ldapHandle, searchResult);

        //entry was found, user with matching credetials exists
        if(ldap_count_entries(ldapHandle, searchResult) > 0){
            userExists = 1;
        }

        //free memory
        ldap_msgfree(searchResult);
        ldap_unbind_ext_s(ldapHandle, NULL, NULL);

        return userExists;
    }

   
}
