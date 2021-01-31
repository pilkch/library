#include <gtest/gtest.h>

#include <spitfire/spitfire.h>
#include <spitfire/communication/http.h>
#include <spitfire/communication/uri.h>

TEST(Spitfire, TestNetworkURI)
{
  const spitfire::network::cURI uri("https://a.b.c.d.e.f.au/first/second/third/fourth/mypage?search=param&x=12345");

  ASSERT_TRUE(uri.IsValidProtocol());
  ASSERT_FALSE(uri.IsValidUsername());
  ASSERT_FALSE(uri.IsValidPassword());
  ASSERT_TRUE(uri.IsValidServer());
  ASSERT_TRUE(uri.IsValidPort());
  ASSERT_TRUE(uri.IsValidPath());

  ASSERT_EQ(spitfire::network::cURI::PROTOCOL::HTTPS, uri.GetProtocol());

  ASSERT_STREQ("", uri.GetUsername().c_str());
  ASSERT_STREQ("", uri.GetPassword().c_str());
  ASSERT_STREQ("a.b.c.d.e.f.au", uri.GetHost().c_str());
  ASSERT_EQ(443, uri.GetPort());
  ASSERT_STREQ("first/second/third/fourth/mypage?search=param&x=12345", uri.GetRelativePath().c_str());
  ASSERT_STREQ("https://a.b.c.d.e.f.au/first/second/third/fourth/mypage?search=param&x=12345", uri.GetFullURI().c_str());
}

TEST(Spitfire, TestNetworkURIEncode)
{
  ASSERT_STREQ("https://a.b.c.d.e.f.au/first/second/third/fourth/mypage?search=param&x=Value+with+spaces", spitfire::network::http::URLEncode("https://a.b.c.d.e.f.au/first/second/third/fourth/mypage?search=param&x=Value with spaces").c_str());


  const spitfire::network::cURI uri(spitfire::network::http::URLEncode("https://a.b.c.d.e.f.au/first/second/third/fourth/mypage?search=param&x=Value with spaces"));

  ASSERT_TRUE(uri.IsValidProtocol());
  ASSERT_FALSE(uri.IsValidUsername());
  ASSERT_FALSE(uri.IsValidPassword());
  ASSERT_TRUE(uri.IsValidServer());
  ASSERT_TRUE(uri.IsValidPort());
  ASSERT_TRUE(uri.IsValidPath());

  ASSERT_EQ(spitfire::network::cURI::PROTOCOL::HTTPS, uri.GetProtocol());

  ASSERT_STREQ("", uri.GetUsername().c_str());
  ASSERT_STREQ("", uri.GetPassword().c_str());
  ASSERT_STREQ("a.b.c.d.e.f.au", uri.GetHost().c_str());
  ASSERT_EQ(443, uri.GetPort());
  ASSERT_STREQ("first/second/third/fourth/mypage?search=param&x=Value+with+spaces", uri.GetRelativePath().c_str());
  ASSERT_STREQ("https://a.b.c.d.e.f.au/first/second/third/fourth/mypage?search=param&x=Value+with+spaces", uri.GetFullURI().c_str());
}
