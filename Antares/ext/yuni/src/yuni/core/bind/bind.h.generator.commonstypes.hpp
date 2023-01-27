		//! The most suitable smartptr for the class
		typedef SmartPtr<BindType> Ptr;

		//! Pointer-to-function
		typedef R (*FunctionType)(<%=generator.list(i)%>);
		//! The type of the return value
		typedef R ReturnType;

		/*!
		** \brief Pointer-to-member of any Class
		** \tparam C Any class
		*/
		template<class C>
		struct PointerToMember final
		{
			//! Mere type for C
			typedef typename Static::Remove::All<C>::Type Class;
			//! Type for a pointer-to-member
			typedef R (Class::*Type)(<%=generator.list(i)%>);
			//! Type for a const pointer-to-member
			typedef R (Class::*ConstType)(<%=generator.list(i)%>) const;
		};
		enum
		{
			//! The number of arguments
			argumentCount = <%=i%>,
		};
		enum
		{
			//! A non-zero value if the prototype has a return value (!= void)
			hasReturnValue = Static::Type::StrictlyEqual<R,void>::No,
		};

		/*!
		** \brief Type of each argument by their index
		**
		** The typedef Type is always valid, even if the argument does not exist.
		** If this case, the type will be the empty class Yuni::None. You may
		** use `argumentCount` to know exactly the number of arguments.
		** \tparam I Index of the argument (zero-based)
		*/
		template<int I>
		struct Argument final
		{
			//! Type of the argument at position I (zero-based)
			typedef typename Private::BindImpl::Argument<R(<%=generator.list(i)%>),I>::Type Type;
		};

		/*!
		** \brief Types when an additional user data is present
		** \tparam U Type for the additional user data
		*/
		template<class U>
		struct WithUserData final
		{
			//! The most suitable type as a parameter
			typedef typename Private::BindImpl::Parameter<U>::Type ParameterType;

			enum
			{
				//! The number of arguments
				argumentCount = <%=i%> + 1, // original count + userdata
			};
			//! A pointer-to-function
			typedef R (*FunctionType)(<%=generator.list(i,'A', "", ", ")%>U);

			/*!
			** \brief Type of each argument by their index
			**
			** The typedef Type is always valid, even if the argument does not exist.
			** If this case, the type will be the empty class Yuni::None. You may
			** use `argumentCount` to know exactly the number of arguments.
			** \tparam I Index of the argument (zero-based)
			*/
			template<int I>
			struct Argument final
			{
				//! Type of the argument at position I
				typedef typename Private::BindImpl::Argument<R(<%=generator.list(i,'A', "", ", ")%>U),I>::Type Type;
			};
		}; // class WithUserData
