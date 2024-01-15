#pragma once

#ifndef __PASS_HPP__
#define __PASS_HPP__

#include "Command.hpp"

class PASS : public Command {
  public:
    PASS();
    PASS(const PASS& p);
    virtual ~PASS();

    PASS& operator=(const PASS& p);

    virtual void               execute(const std::string& first,
                                       const std::string& parameters);
    virtual const std::string& get() const;

  private:
    std::string _password;
};

#endif