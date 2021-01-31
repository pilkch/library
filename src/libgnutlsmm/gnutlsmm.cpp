#include <libgnutlsmm/gnutlsmm.h>

namespace gnutlsmm {
  session::session() :
    s(nullptr)
  {
  }

  session::~session()
  {
    gnutls_deinit(s);
  }

  int session::init(unsigned int flags)
  {
    return gnutls_init(&s, flags);
  }

  gnutls_session_t session::ptr()
  {
    return s;
  }

  int session::bye(gnutls_close_request_t how)
  {
    return gnutls_bye(s, how);
  }

  int session::handshake()
  {
    return gnutls_handshake(s);
  }

  gnutls_alert_description_t session::get_alert() const
  {
    return gnutls_alert_get(s);
  }

  int session::send_alert(gnutls_alert_level_t level, gnutls_alert_description_t desc)
  {
    return gnutls_alert_send(s, level, desc);
  }

  int session::send_appropriate_alert(int err)
  {
    return gnutls_alert_send_appropriate(s, err);
  }

  gnutls_cipher_algorithm_t session::get_cipher() const
  {
    return gnutls_cipher_get(s);
  }

  gnutls_kx_algorithm_t session::get_kx() const
  {
    return gnutls_kx_get(s);
  }

  gnutls_mac_algorithm_t session::get_mac() const
  {
    return gnutls_mac_get(s);
  }

  gnutls_certificate_type_t session::get_certificate_type() const
  {
    return gnutls_certificate_type_get(s);
  }

  void session::set_private_extensions(bool allow)
  {
    gnutls_handshake_set_private_extensions(s, (int) allow);
  }

  gnutls_handshake_description_t session::get_handshake_last_out() const
  {
    return gnutls_handshake_get_last_out(s);
  }

  gnutls_handshake_description_t session::get_handshake_last_in() const
  {
    return gnutls_handshake_get_last_in(s);
  }

  ssize_t session::send(const void *data, size_t sizeofdata)
  {
    return gnutls_record_send(s, data, sizeofdata);
  }

  ssize_t session::recv(void *data, size_t sizeofdata)
  {
    return gnutls_record_recv(s, data, sizeofdata);
  }

  bool session::get_record_direction() const
  {
    return gnutls_record_get_direction(s);
  }

  // maximum packet size
  size_t session::get_max_size() const
  {
    return gnutls_record_get_max_size(s);
  }

  ssize_t session::set_max_size(size_t size)
  {
    return gnutls_record_set_max_size(s, size);
  }

  size_t session::check_pending() const
  {
    return gnutls_record_check_pending(s);
  }


  int session::prf(size_t label_size, const char *label,
		     int server_random_first,
		     size_t extra_size, const char *extra,
		     size_t outsize, char *out)
  {
    return gnutls_prf(s, label_size, label, server_random_first, extra_size, extra, outsize, out);
  }

  int session::prf_raw(size_t label_size, const char *label,
			 size_t seed_size, const char *seed,
			 size_t outsize, char *out)
  {
    return gnutls_prf_raw(s, label_size, label, seed_size, seed, outsize, out);
  }


/* if you just want some defaults, use the following.
 */
  int session::set_priority(const char *prio, const char **err_pos)
  {
    return gnutls_priority_set_direct(s, prio, err_pos);
  }

  int session::set_priority(gnutls_priority_t p)
  {
    return gnutls_priority_set(s, p);
  }

  gnutls_protocol_t session::get_protocol_version() const
  {
    return gnutls_protocol_get_version(s);
  }

  int session::set_data(const void *session_data, size_t session_data_size)
  {
    return gnutls_session_set_data(s, session_data, session_data_size);
  }

  int session::get_data(void *session_data, size_t * session_data_size) const
  {
    return gnutls_session_get_data(s, session_data, session_data_size);
  }

  int session::get_data(gnutls_session_t session, gnutls_datum_t & data) const
  {
    return gnutls_session_get_data2(s, &data);
  }

  int session::get_id(void *session_id, size_t * session_id_size) const
  {
    return gnutls_session_get_id(s, session_id, session_id_size);
  }

  bool session::is_resumed() const
  {
    int ret = gnutls_session_is_resumed(s);

    return (ret != 0);
  }

  int session::verify_peers_certificate(unsigned int &status) const
  {
    return gnutls_certificate_verify_peers2(s, &status);
  }



  int client_session::init()
  {
    return init(GNUTLS_CLIENT);
  }

  int client_session::init(int flags)
  {
    return session::init(GNUTLS_CLIENT | (flags & ~GNUTLS_SERVER));
  }

  void client_session::set_verify_cert(const char *hostname, unsigned flags)
  {
    gnutls_session_set_verify_cert(s, hostname, flags);
  }

  int client_session::set_server_name(gnutls_server_name_type_t type, const void *name, size_t name_length)
  {
    return gnutls_server_name_set(s, type, name, name_length);
  }

  int client_session::get_request_status()
  {
    return gnutls_certificate_client_get_request_status(s);
  }

  const static gnutls_datum_t null_datum = { nullptr, 0 };

  void session::set_max_handshake_packet_length(size_t max)
  {
    gnutls_handshake_set_max_packet_length(s, max);
  }

  void session::clear_credentials()
  {
    gnutls_credentials_clear(s);
  }

  int session::set_credentials(credentials & cred)
  {
    return gnutls_credentials_set(s, cred.get_type(), cred.ptr());
  }

  void session::set_transport_ptr(gnutls_transport_ptr_t ptr)
  {
    gnutls_transport_set_ptr(s, ptr);
  }

  void session::set_transport_ptr(gnutls_transport_ptr_t recv_ptr,
				   gnutls_transport_ptr_t send_ptr)
  {
    gnutls_transport_set_ptr2(s, recv_ptr, send_ptr);
  }


  gnutls_transport_ptr_t session::get_transport_ptr() const
  {
    return gnutls_transport_get_ptr(s);
  }

  void session::get_transport_ptr(gnutls_transport_ptr_t & recv_ptr,
				   gnutls_transport_ptr_t & send_ptr) const
  {
    gnutls_transport_get_ptr2(s, &recv_ptr, &send_ptr);
  }

  void session::set_transport_push_function(gnutls_push_func push_func)
  {
    gnutls_transport_set_push_function(s, push_func);
  }

  void session::set_transport_vec_push_function(gnutls_vec_push_func vec_push_func)
  {
    gnutls_transport_set_vec_push_function(s, vec_push_func);
  }

  void session::set_transport_pull_function(gnutls_pull_func pull_func)
  {
    gnutls_transport_set_pull_function(s, pull_func);
  }

  void session::set_transport_pull_timeout_function(gnutls_pull_timeout_func pull_timeout_func)
  {
    gnutls_transport_set_pull_timeout_function(s, pull_timeout_func);
  }

  void session::set_user_ptr(void *ptr)
  {
    gnutls_session_set_ptr(s, ptr);
  }

  void *session::get_user_ptr() const
  {
    return gnutls_session_get_ptr(s);
  }

  void session::send_openpgp_cert(gnutls_openpgp_crt_status_t status)
  {
#ifdef ENABLE_OPENPGP
    gnutls_openpgp_send_cert(s, status);
#endif
  }

  void session::set_dh_prime_bits(unsigned int bits)
  {
    gnutls_dh_set_prime_bits(s, bits);
  }

  int session::get_dh_secret_bits() const
  {
    return gnutls_dh_get_secret_bits(s);
  }

  int session::get_dh_peers_public_bits() const
  {
    return gnutls_dh_get_peers_public_bits(s);
  }

  int session::get_dh_prime_bits() const
  {
    return gnutls_dh_get_prime_bits(s);
  }

  int session::get_dh_group(gnutls_datum_t & gen, gnutls_datum_t & prime) const
  {
    return gnutls_dh_get_group(s, &gen, &prime);
  }

  int session::get_dh_pubkey(gnutls_datum_t & raw_key) const
  {
    return gnutls_dh_get_pubkey(s, &raw_key);
  }

  gnutls_credentials_type_t session::get_auth_type() const
  {
    return gnutls_auth_get_type(s);
  }

  gnutls_credentials_type_t session::get_server_auth_type() const
  {
    return gnutls_auth_server_get_type(s);
  }

  gnutls_credentials_type_t session::get_client_auth_type() const
  {
    return gnutls_auth_client_get_type(s);
  }


  credentials::credentials(gnutls_credentials_type_t t) : type(t),
    cred(nullptr)
  {
  }

  gnutls_credentials_type_t credentials::get_type() const
  {
    return type;
  }

  void *credentials::ptr() const
  {
    return cred;
  }

  void credentials::set_ptr(void *ptr)
  {
    cred = ptr;
  }


  certificate_credentials::certificate_credentials() : credentials(GNUTLS_CRD_CERTIFICATE), cred(nullptr)
  {
  }

  certificate_credentials::~certificate_credentials()
  {
    gnutls_certificate_free_credentials(cred);
  }

  int certificate_credentials::init()
  {
    const int result = gnutls_certificate_allocate_credentials(&cred);
    if (result < 0) {
      return result;
    }

    set_ptr(cred);

    return 0;
  }

  int certificate_credentials::set_x509_trust_file(const char *cafile, gnutls_x509_crt_fmt_t type)
  {
    return gnutls_certificate_set_x509_trust_file(cred, cafile, type);
  }

  int certificate_credentials::set_x509_trust(const gnutls_datum_t & CA, gnutls_x509_crt_fmt_t type)
  {
    return gnutls_certificate_set_x509_trust_mem(cred, &CA, type);
  }

  int certificate_credentials::set_x509_trust(gnutls_x509_crt_t * ca_list, int ca_list_size)
  {
    return gnutls_certificate_set_x509_trust(cred, ca_list, ca_list_size);
  }
}
