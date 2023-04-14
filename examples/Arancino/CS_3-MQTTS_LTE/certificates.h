#ifndef _CERTIFICATES_H_
#define _CERTIFICATES_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* Board certificate and signature */

const char my_cert[] = 
("-----BEGIN CERTIFICATE-----\r\n"
"YOUR CERTIFICATE LIKE THIS. YOU EASILY CAN GET ONE WITH OPENSSL\r\n"
"openssl x509 -in your_certificate.pem -text"
"-----END CERTIFICATE-----\r\n");

const char my_key[] = 
("-----BEGIN RSA PRIVATE KEY-----\r\n"
"YOUR KEY LIKE THIS AS WELL\r\n"
"-----END RSA PRIVATE KEY-----\r\n");

//Please refer to pycert_bearssl tool for more details on how to generate the rest of this file
// https://github.com/OPEnSLab-OSU/SSLClient/tree/master/tools/pycert_bearssl

#define TAs_NUM 1

static const unsigned char TA_DN0[] = {
    // FIXME
};

static const unsigned char TA_RSA_N0[] = {
    // FIXME
};

static const unsigned char TA_RSA_E0[] = {
    // FIXME
};

static const br_x509_trust_anchor TAs[] = {
    {
        { (unsigned char *)TA_DN0, sizeof TA_DN0 },
        BR_X509_TA_CA,
        {
            BR_KEYTYPE_RSA,
            { .rsa = {
                (unsigned char *)TA_RSA_N0, sizeof TA_RSA_N0,
                (unsigned char *)TA_RSA_E0, sizeof TA_RSA_E0,
            } }
        }
    },
};

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* ifndef _CERTIFICATES_H_ */
