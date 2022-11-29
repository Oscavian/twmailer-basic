#ifndef TWMAILERBASIC_Ldap_H
#define TWMAILERBASIC_Ldap_H
#define Ldap_DEPRECATED 1

#include <string>
#include <iostream>
#include <cstdlib>
#include <ldap.h>
#include <lber.h>
#include <stdlib.h>
#include <stdio.h>
#include <cerrno>

#include "../../share/ClientBase.h"

namespace twServer {

    class Ldap {

    public:
        Ldap(std::string username, std::string password);

        ~Ldap();

        char ldapBindUser[256];
        char ldapBindPassword[256];

        //mostly getters for important ldap parameters
        std::string getUsername() const { return m_username; };

        std::string getPassword() const { return m_password; };

        const char *getLdapURL() const { return ldapURL; };

        const int getLdapVersion() const { return ldapVersion; };

        const char *getLdapSearchBase() const { return ldapSearchBaseDomainComponent; };

        const char *getLdapSearchFilter() const { return ldapSearchFilter; };

        void setLdapSearchFilter(const char *filter) { ldapSearchFilter = filter; };

        ber_int_t getLdapSearchScope() const { return ldapSearchScope; };

        void printSearchResult(LDAP *ldapHandle, LDAPMessage *searchResult);

        int checkIfUserExists();

    private:

        //important ldap parameters
        const char *ldapURL = "ldap://ldap.technikum-wien.at:389";
        const int ldapVersion = LDAP_VERSION3;
        const char *ldapSearchBaseDomainComponent = "dc=technikum-wien,dc=at";
        const char *ldapSearchFilter;
        ber_int_t ldapSearchScope = LDAP_SCOPE_SUBTREE;

        std::string m_username;
        std::string m_password;

    };
}
#endif
