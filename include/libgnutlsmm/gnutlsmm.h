#pragma once

#include <vector>
#include <gnutls/gnutls.h>

namespace gnutlsmm {

	class noncopyable {
    protected:
		noncopyable() {}
        ~noncopyable() {}
    private:
		// These are non-implemented.
        noncopyable(const noncopyable&) = delete;
        noncopyable& operator=(const noncopyable&) = delete;
   	};


	class session:private noncopyable {
	protected:
		gnutls_session_t s;
	public:
		session();
	    virtual ~session();

		int init(unsigned int);
	
		gnutls_session_t ptr();
		int bye(gnutls_close_request_t how);
		int handshake();

		gnutls_alert_description_t get_alert() const;

		int send_alert(gnutls_alert_level_t level,
			       gnutls_alert_description_t desc);
		int send_appropriate_alert(int err);

		gnutls_cipher_algorithm_t get_cipher() const;
		gnutls_kx_algorithm_t get_kx() const;
		gnutls_mac_algorithm_t get_mac() const;
		gnutls_compression_method_t get_compression() const;
		gnutls_certificate_type_t get_certificate_type() const;

		// for the handshake
		void set_private_extensions(bool allow);

		gnutls_handshake_description_t get_handshake_last_out()
		    const;
		gnutls_handshake_description_t get_handshake_last_in()
		    const;

		ssize_t send(const void *data, size_t sizeofdata);
		ssize_t recv(void *data, size_t sizeofdata);

		bool get_record_direction() const;

		// maximum packet size
		size_t get_max_size() const;
		ssize_t set_max_size(size_t size);

		size_t check_pending() const;

		int prf(size_t label_size, const char *label,
			 int server_random_first,
			 size_t extra_size, const char *extra,
			 size_t outsize, char *out);

		int prf_raw(size_t label_size, const char *label,
			     size_t seed_size, const char *seed,
			     size_t outsize, char *out);

		// if you just want some defaults, use the following
		int set_priority(const char *prio, const char **err_pos);
		int set_priority(gnutls_priority_t p);

		gnutls_protocol_t get_protocol_version() const;

		// for resuming sessions
		int set_data(const void *session_data, size_t session_data_size);
		int get_data(void *session_data, size_t* session_data_size) const;
		int get_data(gnutls_session_t session, gnutls_datum_t & data) const;
		int get_id(void *session_id, size_t* session_id_size) const;

		bool is_resumed() const;

		void set_max_handshake_packet_length(size_t max);

		void clear_credentials();
		int set_credentials(class credentials & cred);

		void set_transport_ptr(gnutls_transport_ptr_t ptr);
		void set_transport_ptr(gnutls_transport_ptr_t recv_ptr,
				       gnutls_transport_ptr_t send_ptr);
		gnutls_transport_ptr_t get_transport_ptr() const;
		void get_transport_ptr(gnutls_transport_ptr_t & recv_ptr,
				       gnutls_transport_ptr_t & send_ptr)
		    const;

		void set_transport_push_function(gnutls_push_func
						 push_func);
		void set_transport_vec_push_function(gnutls_vec_push_func
						     vec_push_func);
		void set_transport_pull_function(gnutls_pull_func
						 pull_func);
		void set_transport_pull_timeout_function (gnutls_pull_timeout_func pull_timeout_func);

		void set_user_ptr(void *ptr);
		void *get_user_ptr() const;

		void send_openpgp_cert(gnutls_openpgp_crt_status_t status);

		gnutls_credentials_type_t get_auth_type() const;
		gnutls_credentials_type_t get_server_auth_type() const;
		gnutls_credentials_type_t get_client_auth_type() const;

		// informational stuff
		void set_dh_prime_bits(unsigned int bits);
		int get_dh_secret_bits() const;
		int get_dh_peers_public_bits() const;
		int get_dh_prime_bits() const;
		int get_dh_group(gnutls_datum_t & gen, gnutls_datum_t & prime) const;
		int get_dh_pubkey(gnutls_datum_t & raw_key) const;
		void get_rsa_export_pubkey(gnutls_datum_t & exponent, gnutls_datum_t & modulus) const;

		int verify_peers_certificate(unsigned int &status) const;
	};

	class client_session:public session {
	public:
		int init();
		int init(int flags);

		void set_verify_cert(const char *hostname, unsigned flags);
		int set_server_name(gnutls_server_name_type_t type,
				     const void *name, size_t name_length);

		int get_request_status();
	};


	class credentials:private noncopyable {
    public:
		credentials() : cred(nullptr) {}
		virtual ~credentials() {}

		gnutls_credentials_type_t get_type() const;
    protected:
		friend class session;
		credentials(gnutls_credentials_type_t t);
		void *ptr() const;
		void set_ptr(void *ptr);
		gnutls_credentials_type_t type;
    private:
		void *cred;
	};

	class certificate_credentials:public credentials {
	public:
		certificate_credentials();
		~certificate_credentials();

		int init();

		int set_x509_trust_file(const char *cafile, gnutls_x509_crt_fmt_t type);
		int set_x509_trust(const gnutls_datum_t & CA, gnutls_x509_crt_fmt_t type);
		int set_x509_trust(gnutls_x509_crt_t * ca_list, int ca_list_size);

	protected:
		gnutls_certificate_credentials_t cred;
	};
}
